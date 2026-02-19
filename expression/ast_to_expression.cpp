// #include "ast_to_expression.h"

// static QString opToFunc(const QString& op)
// {
//     if (op == "+")  return "add";
//     if (op == "-")  return "sub";
//     if (op == "*")  return "mul";
//     if (op == "/")  return "div";

//     // Relációs operátorok maradnak operátornak
//     if (op == ">")  return ">";
//     if (op == "<")  return "<";
//     if (op == ">=") return ">=";
//     if (op == "<=") return "<=";
//     if (op == "==") return "==";

//     return op;
// }

// QString AstToExpression::generate(AstNode* node)
// {
//     if (!node)
//         return {};

//     switch (node->type) {

//     case AstNode::Type::Number:
//         return node->value;

//     case AstNode::Type::StringLiteral:
//         return "\"" + node->value + "\"";

//     case AstNode::Type::Variable:
//         return "$" + node->value;

//     case AstNode::Type::Function: {
//         QStringList args;
//         for (AstNode* c : node->children)
//             args << generate(c);
//         return node->value + "(" + args.join(",") + ")";
//     }

//     case AstNode::Type::Operator: {
//         QString op = node->value;

//         if (op == ">" || op == "<" || op == ">=" || op == "<=" || op == "==") {
//             QString left  = generate(node->children[0]);
//             QString right = generate(node->children[1]);
//             return left + op + right;
//         }

//         QString fn = opToFunc(op);
//         QString left  = generate(node->children[0]);
//         QString right = generate(node->children[1]);
//         return fn + "(" + left + "," + right + ")";
//     }

//     case AstNode::Type::Choose: {
//         QString cond      = generate(node->children[0]);
//         QString trueExpr  = generate(node->children[1]);
//         QString falseExpr = generate(node->children[2]);
//         return "(" + cond + " ? " + trueExpr + " : " + falseExpr + ")";
//     }

//     case AstNode::Type::Opt: {
//         QString flag  = generate(node->children[0]);
//         QString value = generate(node->children[1]);
//         return "opt(" + flag + "," + value + ")";
//     }
//     }

//     return {};
// }

