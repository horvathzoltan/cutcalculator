#include "formula_engine.h"
#include "expression/ast_printer.h"
#include "expression/node_pool.h"
#include "tokenizer.h"
#include "parser.h"
#include "ast_builder.h"
#include "variable.h"
#include "function_registry.h"

EvalResult FormulaEngine::eval(const QString& code)
{
    QStringList lines = code.split('\n', Qt::SkipEmptyParts);

    if (lines.size() > 1)
        return evalMultiLine(lines);

    return evalSingleLine(code);
}

inline void dumpTokensAndRpn(const QList<Token>& tokens,
                             const QList<Token>& rpn,
                             QStringList& tokenDumpOut,
                             QStringList& rpnDumpOut)
{
    for (const Token& t : tokens)
        tokenDumpOut << QString("%1('%2') argc=%3")
                            .arg(tokenTypeName(t.type))
                            .arg(t.text)
                            .arg(t.argc);

    for (const Token& t : rpn)
        rpnDumpOut << QString("%1 (%2, argc=%3)")
                          .arg(t.text)
                          .arg(tokenTypeName(t.type))
                          .arg(t.argc);
}

QString dumpAst(AstNode* n, const QString& prefix = "", bool isLast = true)
{
    if (!n) return prefix + "└── <null>\n";

    QString out;
    out += prefix + (isLast ? "└── " : "├── ");
    out += astTypeName(n->type) + "(" + n->value + ")\n";

    for (int i = 0; i < n->children.size(); ++i) {
        bool last = (i == n->children.size() - 1);
        out += dumpAst(n->children[i], prefix + (isLast ? "    " : "│   "), last);
    }

    return out;
}

EvalResult FormulaEngine::evalSingleLine(const QString& code)
{
    NodePool pool;

    // Itt hívjuk a debugolt wrappert:

    Parser::ParseResult pr = Parser::parse(code);

    // Ha akarod, a tokeneket is visszakaphatod külön, de most az RPN a lényeg.
    AstNode* ast = AstBuilder::fromRpn(pr.rpn, pool);

    if (!ast)
        return EvalResult::failure(code, "AST build failed");
    EvalResult r = EvalResult::success();

    try {
        Value result = evalNode(ast, &r);
        VariableRepository::instance().set("_result", result);

        dumpTokensAndRpn(pr.tokens, pr.rpn, r.tokensDump, r.rpnDump);

        QSet<QString> readVars, writtenVars;
        collectVariables(ast, readVars, writtenVars);
        r.readVars = readVars;
        r.writtenVars = writtenVars;
        r.astDump = dumpAst(ast);
        r.variableSnapshot = VariableRepository::instance().dump();

        return r;

    } catch (const QString& err) {
        return EvalResult::failure(code, err);
    } catch (...) {
        return EvalResult::failure(code, "Unknown error");
    }
}



EvalResult FormulaEngine::evalMultiLine(const QStringList& lines)
{
    NodePool pool;

    AstNode* root = pool.create(AstNode::Type::Sequence, "");

    QStringList allTokenDump;
    QStringList allRpnDump;
    QString code = lines.join("\n");

    for (int i = 0; i < lines.size(); ++i) {
        QString trimmed = lines[i].trimmed();
        if (trimmed.isEmpty())
            continue;

        // auto tokens = Tokenizer::tokenize(trimmed);
        // auto rpn    = Parser::toRpn(tokens);
        // AstNode* ast = AstBuilder::fromRpn(rpn, pool);

        Parser::ParseResult pr = Parser::parse(trimmed);
        AstNode* ast = AstBuilder::fromRpn(pr.rpn, pool);
        dumpTokensAndRpn(pr.tokens, pr.rpn, allTokenDump, allRpnDump);

        if (!ast)
            return EvalResult::failure(code, "AST build failed in multi-line script");

        root->children.append(ast);

        dumpTokensAndRpn(pr.tokens, pr.rpn, allTokenDump, allRpnDump);
    }

    EvalResult r = EvalResult::success();

    try {
        Value result = evalNode(root, &r);
        VariableRepository::instance().set("_result", result);

        r.tokensDump = allTokenDump;
        r.rpnDump    = allRpnDump;

        QSet<QString> readVars, writtenVars;
        collectVariables(root, readVars, writtenVars);
        r.readVars = readVars;
        r.writtenVars = writtenVars;
        r.astDump = dumpAst(root); // 🔥 EZ HIÁNYZOTT
        r.variableSnapshot = VariableRepository::instance().dump();

        return r;

    } catch (const QString& err) {
        return EvalResult::failure(code, err);
    } catch (...) {
        return EvalResult::failure(code, "Unknown error");
    }
}




/**/


QVector<Value> FormulaEngine::evalChildren(AstNode* n, EvalResult* traceOut)
{
    QVector<Value> out;
    for (int i = 0; i < n->children.size(); ++i)
        out.append(evalNode(n->children[i], traceOut));
    return out;
}


Value FormulaEngine::evalNode(AstNode* n, EvalResult* traceOut)
{
    auto& vars = VariableRepository::instance();
    auto& reg  = FunctionRegistry::instance();

    auto addTrace = [&](const Value& v) {
        if (!traceOut) return;
        TraceEntry e;
        e.nodeType  = astTypeName(n->type);
        e.nodeValue = n->value;
        e.result    = v.toString();
        traceOut->trace.append(e);
    };

    switch (n->type) {

    case AstNode::Type::Number: {
        Value v = Value::numberValue(n->value.toDouble());
        addTrace(v);
        return v;
    }

    case AstNode::Type::StringLiteral: {
        Value v = Value::stringValue(n->value);
        addTrace(v);
        return v;
    }

    case AstNode::Type::Variable: {
        Value v = vars.get(n->value);
        if (v.type == Value::Type::Null)
            throw QString("Undefined variable: %1").arg(n->value);
        addTrace(v);
        return v;
    }

    case AstNode::Type::Operator: {
        QVector<Value> args = evalChildren(n, traceOut);

        QVector<Value> filtered;
        for (const Value& a : args)
            if (a.type != Value::Type::Skip)
                filtered.append(a);

        if (filtered.isEmpty()) {
            Value v = Value::nullValue();
            addTrace(v);
            return v;
        }

        if (filtered.size() == 1) {
            Value v = filtered[0];
            addTrace(v);
            return v;
        }

        Value v = reg.call(n->value, filtered);
        addTrace(v);
        return v;

    }

    case AstNode::Type::Function: {
        QVector<Value> args = evalChildren(n, traceOut);

        QVector<Value> filtered;
        for (const Value& a : args)
            if (a.type != Value::Type::Skip)
                filtered.append(a);

        Value v = reg.call(n->value, filtered);
        addTrace(v);
        return v;
    }

    case AstNode::Type::Assignment: {
        Value v = evalNode(n->children[0], traceOut);
        vars.set(n->value, v);
        Value ret = Value::nullValue();
        addTrace(ret);
        return ret;
    }

    case AstNode::Type::Return: {
        Value last = Value::nullValue();
        for (int i = 0; i < n->children.size(); ++i) {
            last = evalNode(n->children[i], traceOut);
        }
        addTrace(last);
        return last;
    }

    // cond ? trueExpr : falseExpr
    case AstNode::Type::Choose: {
        if (n->children.size() != 3) return Value();

        Value cond = evalNode(n->children[0], traceOut);
        addTrace(cond);
        if (cond.toBool()){
            auto v = evalNode(n->children[1], traceOut);
            addTrace(v);
            return v;
        }
        else{
            auto v = evalNode(n->children[2], traceOut);
            addTrace(v);
            return v;
        }
    }

    case AstNode::Type::Opt: {
        if (n->children.size() != 2) return Value();

        Value cond = evalNode(n->children[0], traceOut);
        addTrace(cond);
        if (cond.toBool()){
            auto v = evalNode(n->children[1], traceOut);
            addTrace(v);
            return v;
        }
        else
            return Value::skipValue();
    }


    case AstNode::Type::Statement: {
        Value v = evalNode(n->children[0], traceOut);
        addTrace(v);
        return v;
    }

    case AstNode::Type::Sequence: {
        Value last = Value::nullValue();
        for (int i = 0; i < n->children.size(); ++i)
            last = evalNode(n->children[i], traceOut);
        addTrace(last);
        return last;
    }

    }

    Value v = Value::nullValue();
    addTrace(v);
    return v;
}

