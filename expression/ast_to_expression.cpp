#include "ast_to_expression.h"

static QString opToFunc(const QString& op)
{
    if (op == "+")  return "add";
    if (op == "-")  return "sub";
    if (op == "*")  return "mul";
    if (op == "/")  return "div";

    // Relációs operátorok maradnak operátornak
    if (op == ">")  return ">";
    if (op == "<")  return "<";
    if (op == ">=") return ">=";
    if (op == "<=") return "<=";
    if (op == "==") return "==";

    return op;
}

QString AstToExpression::generate(AstNode* node)
{
    if (!node)
        return {};

    switch (node->type) {

    case AstNode::Type::Number:
        return node->value;

    case AstNode::Type::Variable:
        return "$" + node->value;

    case AstNode::Type::Function: {
        // Függvényhívás: fn(arg1, arg2, ...)
        QStringList args;
        for (AstNode* c : node->children)
            args << generate(c);
        return node->value + "(" + args.join(",") + ")";
    }

    case AstNode::Type::Operator: {
        QString op = node->value;

        // Relációs operátor → infix formában marad
        if (op == ">" || op == "<" || op == ">=" || op == "<=" || op == "==") {
            QString left  = generate(node->children[0]);
            QString right = generate(node->children[1]);
            return left + op + right;
        }

        // Aritmetikai operátor → függvény
        QString fn = opToFunc(op);
        QString left  = generate(node->children[0]);
        QString right = generate(node->children[1]);
        return fn + "(" + left + "," + right + ")";
    }
    }

    return {};
}
