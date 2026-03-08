#include "formula_engine.h"
#include "expression/ast_printer.h"
#include "expression/node_pool.h"
//#include "tokenizer.h"
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

    // 1) Parse
    auto pr = Parser::parse(code);
    if (!pr.ok)
        return EvalResult::failure(code, "Szintaktikai hiba: " + pr.error);

    // 2) AST build
    auto astRes = AstBuilder::fromRpn(pr.value.rpn, pool);
    if (!astRes.ok)
        return EvalResult::failure(code, "AST build hiba: " + astRes.error);

    AstNode* ast = astRes.value;

    EvalResult r = EvalResult::success();

    // 3) Eval (ez még dobhat)
    Result<Value> result = evalNode(ast, &r);
    if (!result.ok)
        return EvalResult::failure(code, result.error);

    VariableRepository::instance().set("_result", result.value);

    dumpTokensAndRpn(pr.value.tokens, pr.value.rpn, r.tokensDump, r.rpnDump);

    QSet<QString> readVars, writtenVars;
    collectVariables(ast, readVars, writtenVars);
    r.readVars = readVars;
    r.writtenVars = writtenVars;
    r.astDump = dumpAst(ast);
    r.variableSnapshot = VariableRepository::instance().dump();

    return r;
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

        // 1) Parse
        auto pr = Parser::parse(trimmed);
        if (!pr.ok)
            return EvalResult::failure(code, "Szintaktikai hiba: " + pr.error);

        // 2) AST build
        auto astRes = AstBuilder::fromRpn(pr.value.rpn, pool);
        if (!astRes.ok)
            return EvalResult::failure(code, "AST build hiba: " + astRes.error);

        AstNode* ast = astRes.value;

        dumpTokensAndRpn(pr.value.tokens, pr.value.rpn, allTokenDump, allRpnDump);

        root->children.append(ast);
    }

    EvalResult r = EvalResult::success();

    // 3) Eval
    Result<Value> result = evalNode(root, &r);
    if (!result.ok)
        return EvalResult::failure(code, result.error);

    VariableRepository::instance().set("_result", result.value);

    r.tokensDump = allTokenDump;
    r.rpnDump    = allRpnDump;

    QSet<QString> readVars, writtenVars;
    collectVariables(root, readVars, writtenVars);
    r.readVars = readVars;
    r.writtenVars = writtenVars;
    r.astDump = dumpAst(root);
    r.variableSnapshot = VariableRepository::instance().dump();

    return r;

}




/**/


Result<QVector<Value>> FormulaEngine::evalChildren(AstNode* n, EvalResult* traceOut)
{
    QVector<Value> out;

    const QVector<AstNode*>& children = n->children;
    for (AstNode* child : children) {
        Result<Value> r = evalNode(child, traceOut);
        if (!r.ok)
            return Result<QVector<Value>>::failure(r.error);
        out.append(r.value);
    }

    return Result<QVector<Value>>::success(out);
}


Result<Value> FormulaEngine::evalNode(AstNode* n, EvalResult* traceOut)
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

    // Helper: evaluate a child safely
    auto evalChild = [&](AstNode* c) -> Result<Value> {
        return evalNode(c, traceOut);
    };

    // Helper: evaluate all children safely
    auto evalChildrenSafe = [&](AstNode* node) -> Result<QVector<Value>> {
        QVector<Value> out;
        const auto& children = node->children;
        for (AstNode* c : children) {
            auto r = evalNode(c, traceOut);
            if (!r.ok) return Result<QVector<Value>>::failure(r.error);
            out.append(r.value);
        }
        return Result<QVector<Value>>::success(out);
    };

    switch (n->type) {

    case AstNode::Type::Number: {
        Value v = Value::numberValue(n->value.toDouble());
        addTrace(v);
        return Result<Value>::success(v);
    }

    case AstNode::Type::StringLiteral: {
        Value v = Value::stringValue(n->value);
        addTrace(v);
        return Result<Value>::success(v);
    }

    case AstNode::Type::Variable: {
        Value v = vars.get(n->value);
        if (v.type == Value::Type::Null)
            return Result<Value>::failure("Undefined variable: " + n->value);
        addTrace(v);
        return Result<Value>::success(v);
    }

    case AstNode::Type::Operator: {
        auto argsRes = evalChildrenSafe(n);
        if (!argsRes.ok) return Result<Value>::failure(argsRes.error);

        QVector<Value> filtered;
        const auto& values = argsRes.value;
        for (const Value& a : values) {
            if (a.type != Value::Type::Skip)
                filtered.append(a);
        }

        if (filtered.isEmpty()) {
            Value v = Value::nullValue();
            addTrace(v);
            return Result<Value>::success(v);
        }

        if (filtered.size() == 1) {
            Value v = filtered[0];
            if (v.type == Value::Type::Error)
                return Result<Value>::failure(v.text);
            addTrace(v);
            return Result<Value>::success(v);
        }

        Result<Value> callRes = reg.call(n->value, filtered);
        if (!callRes.ok) return Result<Value>::failure(callRes.error);

        addTrace(callRes.value);
        return Result<Value>::success(callRes.value);
    }

    case AstNode::Type::Function: {
        auto argsRes = evalChildrenSafe(n);
        if (!argsRes.ok) return Result<Value>::failure(argsRes.error);

        QVector<Value> filtered;
        const auto& values = argsRes.value;
        for (const Value& a : values){
            if (a.type != Value::Type::Skip)
                filtered.append(a);
        }

        Result<Value> callRes = reg.call(n->value, filtered);
        if (!callRes.ok) return Result<Value>::failure(callRes.error);

        addTrace(callRes.value);
        return Result<Value>::success(callRes.value);
    }

    case AstNode::Type::Assignment: {
        auto r = evalNode(n->children[0], traceOut);
        if (!r.ok) return r;

        vars.set(n->value, r.value);
        Value ret = Value::nullValue();
        addTrace(ret);
        return Result<Value>::success(ret);
    }

    case AstNode::Type::Return: {
        Value last = Value::nullValue();
        for (AstNode* c : n->children) {
            auto r = evalNode(c, traceOut);
            if (!r.ok) return r;
            last = r.value;
        }
        addTrace(last);
        return Result<Value>::success(last);
    }

    case AstNode::Type::Choose: {
        if (n->children.size() != 3)
            return Result<Value>::failure("Invalid choose node");

        auto condRes = evalNode(n->children[0], traceOut);
        if (!condRes.ok) return condRes;
        addTrace(condRes.value);

        int idx = condRes.value.toBool() ? 1 : 2;
        auto r = evalNode(n->children[idx], traceOut);
        if (!r.ok) return r;

        if (r.value.type == Value::Type::Error)
            return Result<Value>::failure(r.value.text);

        addTrace(r.value);
        return Result<Value>::success(r.value);

    }

    case AstNode::Type::Opt: {
        if (n->children.size() != 2)
            return Result<Value>::failure("Invalid opt node");

        auto condRes = evalNode(n->children[0], traceOut);
        if (!condRes.ok) return condRes;
        addTrace(condRes.value);

        if (condRes.value.toBool()) {
            auto r = evalNode(n->children[1], traceOut);
            if (!r.ok) return r;

            if (r.value.type == Value::Type::Error)
                return Result<Value>::failure(r.value.text);

            addTrace(r.value);
            return Result<Value>::success(r.value);

        }

        return Result<Value>::success(Value::skipValue());
    }

    case AstNode::Type::Statement: {
        auto r = evalNode(n->children[0], traceOut);
        if (!r.ok) return r;
        addTrace(r.value);
        return r;
    }

    case AstNode::Type::Sequence: {
        Value last = Value::nullValue();
        for (AstNode* c : n->children) {
            auto r = evalNode(c, traceOut);
            if (!r.ok) return r;
            last = r.value;
        }
        addTrace(last);
        return Result<Value>::success(last);
    }
    }

    return Result<Value>::failure("Unknown AST node type");
}
