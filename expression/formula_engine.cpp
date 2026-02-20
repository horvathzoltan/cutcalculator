#include "formula_engine.h"
#include "expression/node_pool.h"
#include "tokenizer.h"
#include "parser.h"
#include "ast_builder.h"
#include "variable.h"
#include "function_registry.h"

EvalResult FormulaEngine::eval(const QString& code)
{
    // Több soros script: soronként értékeljük, a végső sor eredménye lesz _result
    if (code.contains('\n')) {
        QStringList lines = code.split('\n', Qt::SkipEmptyParts);
        Value last = Value::nullValue();

        for (const QString& line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed.isEmpty())
                continue;

            auto r = eval(trimmed);
            if (!r.ok)
                return r;

            last = VariableRepository::instance().get("_result");
        }

        VariableRepository::instance().set("_result", last);
        return EvalResult::success();
    }

    NodePool pool;
    auto tokens = Tokenizer::tokenize(code);
    auto rpn    = Parser::toRpn(tokens);
    AstNode* ast = AstBuilder::fromRpn(rpn, pool);

    if (!ast)
        return EvalResult::failure("AST build failed");

    try {
        Value result = evalNode(ast);
        VariableRepository::instance().set("_result", result);

        return EvalResult::success();
    } catch (const QString& err) {
        return EvalResult::failure(err);
    } catch (...) {
        return EvalResult::failure("Unknown error");
    }
}


QVector<Value> FormulaEngine::evalChildren(AstNode* n)
{
    QVector<Value> out;
    for (AstNode* c : n->children)
        out.append(evalNode(c));
    return out;
}

Value FormulaEngine::evalNode(AstNode* n)
{
    auto& vars = VariableRepository::instance();
    auto& reg  = FunctionRegistry::instance();

    switch (n->type) {

    case AstNode::Type::Number:
        return Value::numberValue(n->value.toDouble());

    case AstNode::Type::StringLiteral:
        return Value::stringValue(n->value);

    case AstNode::Type::Variable:
    {
        Value v = vars.get(n->value);
        if (v.type == Value::Type::Null)
            throw QString("Undefined variable: %1").arg(n->value);
        return v;
    }


    case AstNode::Type::Operator:
        return reg.call(n->value, evalChildren(n));

    case AstNode::Type::Function:
        return reg.call(n->value, evalChildren(n));

    case AstNode::Type::Assignment: {
        Value v = evalNode(n->children[0]);
        vars.set(n->value, v);
        return Value::nullValue();
    }

    case AstNode::Type::Return: {
        Value last = Value::nullValue();
        for (AstNode* a : n->children) {
            Value v = evalNode(a);
            last = v;
        }
        return last;
    }

    case AstNode::Type::Choose: {
        Value cond = evalNode(n->children[0]);
        if (cond.type == Value::Type::Bool && cond.boolean)
            return evalNode(n->children[1]);
        else
            return evalNode(n->children[2]);
    }

    case AstNode::Type::Opt: {
        Value flag = evalNode(n->children[0]);
        if (flag.type == Value::Type::Number && flag.number != 0.0)
            return evalNode(n->children[1]);
        return Value::numberValue(0.0);
    }

    case AstNode::Type::Statement:
        return evalNode(n->children[0]);

    case AstNode::Type::Sequence: {
        Value last = Value::nullValue();
        for (AstNode* stmt : n->children)
            last = evalNode(stmt);
        return last;
    }

    }

    return Value::nullValue();
}
