#include "ast_builder.h"
#include <QStack>

AstNode* AstBuilder::fromRpn(const QVector<Token>& rpn, NodePool& pool)
{
    QStack<AstNode*> stack;

    for (const Token& t : rpn) {

        // Literálok
        if (t.type == TokenType::Number) {
            stack.push(pool.create(AstNode::Type::Number, t.text));
            continue;
        }

        if (t.type == TokenType::StringLiteral) {
            stack.push(pool.create(AstNode::Type::StringLiteral, t.text));
            continue;
        }


        if (t.type == TokenType::Variable) {
            stack.push(pool.create(AstNode::Type::Variable, t.text));
            continue;
        }

        // Függvény
        // --- Függvények ---
        if (t.type == TokenType::Function) {

            // --- choose(cond, trueExpr, falseExpr) ---
            if (t.text == "choose") {
                if (stack.size() < 3) {
                    qWarning() << "AstBuilder: choose() requires 3 arguments";
                    return nullptr;
                }
                AstNode* falseExpr = stack.pop();
                AstNode* trueExpr  = stack.pop();
                AstNode* cond      = stack.pop();

                AstNode* node = pool.create(AstNode::Type::Choose, "choose");
                node->children.append(cond);
                node->children.append(trueExpr);
                node->children.append(falseExpr);

                stack.push(node);
                continue;
            }

            // --- opt(flag, value) ---
            if (t.text == "opt") {
                if (stack.size() < 2) {
                    qWarning() << "AstBuilder: opt() requires 2 arguments";
                    return nullptr;
                }
                AstNode* value = stack.pop();
                AstNode* flag  = stack.pop();

                AstNode* node = pool.create(AstNode::Type::Opt, "opt");
                node->children.append(flag);
                node->children.append(value);

                stack.push(node);
                continue;
            }

            // --- fallback: 1 paraméteres függvény ---
            if (stack.isEmpty()) {
                qWarning() << "AstBuilder: function" << t.text << "has no argument";
                return nullptr;
            }

            AstNode* arg = stack.pop();
            AstNode* fn = pool.create(AstNode::Type::Function, t.text);
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

            AstNode* op = pool.create(AstNode::Type::Operator, t.text);
            op->children.append(left);
            op->children.append(right);

            stack.push(op);
            continue;
        }

        // Assignment: name = expr
        if (t.type == TokenType::Assign) {
            AstNode* expr = stack.pop();
            AstNode* var  = stack.pop();
            AstNode* node = pool.create(AstNode::Type::Assignment, var->value);
            node->children.append(expr);
            stack.push(node);
            continue;
        }


        // Return: return a=1, b=2, ...
        if (t.type == TokenType::Return) {
            QVector<AstNode*> assignments;
            while (!stack.isEmpty() && stack.top()->type == AstNode::Type::Assignment) {
                assignments.prepend(stack.pop());
            }
            AstNode* node = pool.create(AstNode::Type::Return, "return");
            node->children = assignments;
            stack.push(node);
            continue;
        }


        // End vagy Unknown → ignoráljuk
    }

    if (stack.isEmpty())
        return nullptr;

    return stack.pop();
}
