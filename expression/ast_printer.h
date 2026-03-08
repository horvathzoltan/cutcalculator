#pragma once
#include "ast.h"
#include "expression/token.h"
// #include "common/logger/logger.h"
// #include "expression/eval_result.h"
// #include "value.h"
//#include <QDebug>

inline QString astTypeName(AstNode::Type t)
{
    switch (t) {
    case AstNode::Type::Number:       return "Number";
    case AstNode::Type::Variable:     return "Variable";
    case AstNode::Type::Function:     return "Function";
    case AstNode::Type::Operator:     return "Operator";
    case AstNode::Type::Choose:       return "Choose";
    case AstNode::Type::Opt:          return "Opt";
    case AstNode::Type::StringLiteral:return "String";
    case AstNode::Type::Assignment:   return "Assignment";
    case AstNode::Type::Return:       return "Return";
    case AstNode::Type::Sequence:     return "Sequence";
    case AstNode::Type::Statement:    return "Statement";
    }
    return "Unknown";
}

// inline void printAstNode(AstNode* n, int indent = 0)
// {
//     if (!n) {
//         zInfo() << QString(indent, ' ') + "<null>";
//         return;
//     }

//     QString ind(indent, ' ');
//     zInfo()
//         << ind
//         << astTypeName(n->type)
//         << "(" << n->value << ")";

//     for (int i = 0; i < n->children.size(); ++i) {
//         printAstNode(n->children[i], indent + 2);
//     }
// }

// inline void printAstTree(AstNode* node,
//                          const QString& prefix = "",
//                          bool isLast = true,
//                          bool isRoot = true)
// {
//     if (isRoot)
//         zInfo() << "\n=== AST TREE ===";

//     if (!node) {
//         zInfo() << prefix << (isLast ? "└── " : "├── ") << "<null>";
//         return;
//     }

//     QString connector = isLast ? "└── " : "├── ";

//     // Node header
//     zInfo()
//         << prefix
//         << connector
//         << astTypeName(node->type)
//         << "(" << node->value << ")";

//     // Gyerekek
//     const auto& kids = node->children;
//     for (int i = 0; i < kids.size(); ++i) {
//         bool lastChild = (i == kids.size() - 1);
//         QString newPrefix = prefix + (isLast ? "    " : "│   ");
//         printAstTree(kids[i], newPrefix, lastChild, false);
//     }

//     if (isRoot)
//         zInfo() << "================\n";
// }

inline void collectVariables(AstNode* n,
                             QSet<QString>& readVars,
                             QSet<QString>& writtenVars)
{
    if (!n) return;

    switch (n->type) {

    case AstNode::Type::Variable:
        readVars.insert(n->value);
        break;

    case AstNode::Type::Assignment:
        // assignment node value = variable name
        writtenVars.insert(n->value);
        // right side expression still needs to be scanned
        break;

    default:
        break;
    }

    for (int i = 0; i < n->children.size(); ++i)
        collectVariables(n->children[i], readVars, writtenVars);
}


// inline void printVariables(const QMap<QString, Value>& vars,
//                            const QString& title = "Variables")
// {
//     zInfo() << "\n=== " << title << " ===";

//     if (vars.isEmpty()) {
//         zInfo() << "(empty)";
//         return;
//     }

//     QStringList keys = vars.keys();
//     keys.sort();

//     for (int i = 0; i < keys.size(); ++i) {
//         const QString& key = keys[i];
//         const Value& val = vars[key];

//         QString repr;

//         switch (val.type) {
//         case Value::Type::Number: repr = QString::number(val.number); break;
//         case Value::Type::Bool:   repr = val.boolean ? "true" : "false"; break;
//         case Value::Type::String: repr = "\"" + val.text + "\""; break;
//         case Value::Type::Null:   repr = "null"; break;
//         }

//         zInfo() << " " << key << "=" << repr;
//     }

//     zInfo() << "====================\n";
// }

inline QString tokenTypeName(TokenType t)
{
    switch (t) {

    // --- Literálok ---
    case TokenType::Number:         return "Number";
    case TokenType::StringLiteral:  return "StringLiteral";

    // --- Azonosítók ---
    case TokenType::Variable:       return "Variable";
    case TokenType::Function:       return "Function";
    case TokenType::Identifier:     return "Identifier";

    // --- DSL kulcsszavak ---
    case TokenType::TernaryOp:      return "TernaryOp";
    case TokenType::OptionalOp:     return "OptionalOp";

    // --- Operátorok ---
    case TokenType::Plus:           return "Plus";
    case TokenType::Minus:          return "Minus";
    case TokenType::PrefixPlus:     return "PrefixPlus";
    case TokenType::PrefixMinus:    return "PrefixMinus";

    case TokenType::Star:           return "Star";
    case TokenType::Slash:          return "Slash";

    case TokenType::Greater:        return "Greater";
    case TokenType::Less:           return "Less";
    case TokenType::GreaterEqual:   return "GreaterEqual";
    case TokenType::LessEqual:      return "LessEqual";
    case TokenType::Equal:          return "Equal";

    case TokenType::TernaryQuestion:   return "TernaryQuestion";
    case TokenType::OptionalQuestion:  return "OptionalQuestion";

    case TokenType::Colon:          return "Colon";
    case TokenType::Comma:          return "Comma";
    case TokenType::LParen:         return "LParen";
    case TokenType::RParen:         return "RParen";

    case TokenType::Assign:         return "Assign";
    case TokenType::Return:         return "Return";

    case TokenType::Newline:        return "Newline";
    case TokenType::StatementEnd:   return "StatementEnd";
    case TokenType::End:            return "End";

    case TokenType::Unknown:        return "Unknown";
    }

    return "InvalidToken";
}



// inline void printTokens(const QList<Token>& tokens,
//                         const QString& title = "Tokens")
// {
//     zInfo() << "\n=== " << title << " ===";

//     if (tokens.isEmpty()) {
//         zInfo() << "(empty)";
//         return;
//     }

//     for (int i = 0; i < tokens.size(); ++i) {
//         const Token& t = tokens[i];
//         zInfo() << " "
//                 << tokenTypeName(t.type)
//                 << "('" << t.text << "')"
//                 << " argc=" << t.argc;
//     }


//     zInfo() << "====================\n";
// }



// inline void printRpn(const QList<Token>& rpn,
//                      const QString& title = "RPN")
// {
//     zInfo() << "\n=== " << title << " ===";

//     if (rpn.isEmpty()) {
//         zInfo() << "(empty)";
//         return;
//     }

//     for (int i = 0; i < rpn.size(); ++i) {
//         const Token& t = rpn[i];
//         zInfo() << " "
//                 << t.text
//                 << " (" << tokenTypeName(t.type)
//                 << ", argc=" << t.argc << ")";
//     }

//     zInfo() << "====================\n";
// }

// inline QString setToString(const QSet<QString>& s)
// {
//     QStringList list = QStringList(s.begin(), s.end());
//     list.sort();
//     return "{" + list.join(", ") + "}";
// }

// inline void debugPipeline(const QString& code, const EvalResult& r)
// {
//     zInfo() << "\n================ DEBUG PIPELINE ================";
//     zInfo() << "Code:" << code;

//     // Token dump
//     if (!r.tokensDump.isEmpty()) {
//         zInfo() << "\n=== Tokens ===";
//         for (const QString& s : r.tokensDump)
//             zInfo() << " " << s;
//     }

//     // RPN dump
//     if (!r.rpnDump.isEmpty()) {
//         zInfo() << "\n=== RPN ===";
//         for (const QString& s : r.rpnDump)
//             zInfo() << " " << s;
//     }

//     // AST
//     //printAstTree(r.ast);
//     // AST dump
//     if (!r.astDump.isEmpty()) {
//         zInfo() << "\n=== AST DUMP ===" << "\n" << r.astDump;
//     }


//     // Variables
//     zInfo() << "Read variables:" << setToString(r.readVars);
//     zInfo() << "Written variables:" << setToString(r.writtenVars);

//     // Execution trace
//     if (!r.trace.isEmpty()) {
//         zInfo() << "\n=== Execution Trace ===";
//         for (int i = 0; i < r.trace.size(); ++i) {
//             const TraceEntry& te = r.trace[i];
//             zInfo() << " " << te.nodeType << "(" << te.nodeValue << ") => " << te.result;
//         }
//     }

//     zInfo() << "================================================\n";
// }



