#include "ast_builder.h"
#include <QStack>

AstNode* AstBuilder::fromRpn(const QVector<Token>& rpn, NodePool& pool)
{
    QStack<AstNode*> stack;
    QVector<AstNode*> statements;

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

        if (t.type == TokenType::Choose) {
            if (stack.size() < 3)
                return nullptr;

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

        if (t.type == TokenType::Opt) {
            if (stack.size() < 2)
                return nullptr;

            AstNode* value = stack.pop();
            AstNode* flag  = stack.pop();

            AstNode* node = pool.create(AstNode::Type::Opt, "opt");
            node->children.append(flag);
            node->children.append(value);

            stack.push(node);
            continue;
        }

        // Függvény
        // --- Függvények ---
        if (t.type == TokenType::Function) {
            int argc = t.argc;

            if (stack.size() < argc) {
                qWarning() << "AstBuilder: function" << t.text
                           << "requires" << argc << "arguments, but stack has"
                           << stack.size();
                return nullptr;
            }

            AstNode* fn = pool.create(AstNode::Type::Function, t.text);

            QVector<AstNode*> args;
            for (int i = 0; i < argc; ++i)
                args.prepend(stack.pop());   // helyes sorrend

            fn->children = args;
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


        if (t.type == TokenType::StatementEnd) {
            if (stack.isEmpty())
                return nullptr;

            AstNode* expr = stack.pop();
            AstNode* stmt = pool.create(AstNode::Type::Statement, "");
            stmt->children.append(expr);
            statements.append(stmt);
            continue;
        }

        // End vagy Unknown → ignoráljuk
    }

    if (stack.isEmpty())
        return nullptr;

    if (!statements.isEmpty()) {
        AstNode* expr = stack.pop();
        AstNode* stmt = pool.create(AstNode::Type::Statement, "");
        stmt->children.append(expr);
        statements.append(stmt);

        if (statements.size() == 1)
            return statements[0];

        AstNode* seq = pool.create(AstNode::Type::Sequence, "");
        seq->children = statements;
        return seq;
    }

    return stack.pop();

}
