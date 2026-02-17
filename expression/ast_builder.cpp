#include "ast_builder.h"
#include <QStack>

AstNode* AstBuilder::fromRpn(const QVector<Token>& rpn)
{
    QStack<AstNode*> stack;

    for (const Token& t : rpn) {

        // Literálok
        if (t.type == TokenType::Number) {
            stack.push(new AstNode(AstNode::Type::Number, t.text));
            continue;
        }

        if (t.type == TokenType::Variable) {
            stack.push(new AstNode(AstNode::Type::Variable, t.text));
            continue;
        }

        // Függvény
        if (t.type == TokenType::Function) {
            // A függvény paraméterei már az AST stack tetején vannak
            // DE: a paraméterszámot nem tudjuk a tokenből → később megoldjuk
            // Most: 1 paraméteres függvényként kezeljük (később bővítjük)
            AstNode* arg = stack.pop();
            AstNode* fn = new AstNode(AstNode::Type::Function, t.text);
            fn->children.append(arg);
            stack.push(fn);
            continue;
        }

        // Operátorok
        if (t.type == TokenType::Plus ||
            t.type == TokenType::Minus ||
            t.type == TokenType::Star ||
            t.type == TokenType::Slash ||
            t.type == TokenType::Greater ||
            t.type == TokenType::Less ||
            t.type == TokenType::GreaterEqual ||
            t.type == TokenType::LessEqual ||
            t.type == TokenType::Equal)
        {
            AstNode* right = stack.pop();
            AstNode* left  = stack.pop();

            AstNode* op = new AstNode(AstNode::Type::Operator, t.text);
            op->children.append(left);
            op->children.append(right);

            stack.push(op);
            continue;
        }

        // End vagy Unknown → ignoráljuk
    }

    if (stack.isEmpty())
        return nullptr;

    return stack.pop();
}
