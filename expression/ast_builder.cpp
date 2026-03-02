#include "ast_builder.h"
#include "common/utils/result.h"
#include <QStack>

Result<AstNode*> AstBuilder::fromRpn(const QVector<Token>& rpn, NodePool& pool)
{
    QStack<AstNode*> stack;
    QVector<AstNode*> statements;

    auto fail = [&](const QString& msg) {
        return Result<AstNode*>::failure(msg);
    };

    for (const Token& t : rpn) {

        // --- Literálok ---
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

        // --- Choose ---
        if (t.type == TokenType::Choose) {
            if (stack.size() < 3)
                return fail("Hibás ternary (choose) szerkezet");

            AstNode* falseExpr = stack.pop();
            AstNode* trueExpr  = stack.pop();
            AstNode* cond      = stack.pop();

            AstNode* node = pool.create(AstNode::Type::Choose, "choose");
            node->children = { cond, trueExpr, falseExpr };

            stack.push(node);
            continue;
        }

        // --- Opt ---
        if (t.type == TokenType::Opt) {
            if (stack.size() < 2)
                return fail("Hibás opt szerkezet (flag ? expr)");

            AstNode* value = stack.pop();
            AstNode* flag  = stack.pop();

            AstNode* node = pool.create(AstNode::Type::Opt, "opt");
            node->children = { flag, value };

            stack.push(node);
            continue;
        }

        // --- Függvény ---
        if (t.type == TokenType::Function) {
            int argc = t.argc;

            if (stack.size() < argc)
                return fail(QString("A(z) %1 függvény %2 argumentumot vár, de csak %3 van")
                                .arg(t.text).arg(argc).arg(stack.size()));

            AstNode* fn = pool.create(AstNode::Type::Function, t.text);

            QVector<AstNode*> args;
            for (int i = 0; i < argc; ++i)
                args.prepend(stack.pop());

            fn->children = args;
            stack.push(fn);
            continue;
        }

        // --- Operátorok ---
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
            if (stack.size() < 2)
                return fail(QString("Operátor '%1' túl kevés operandussal").arg(t.text));

            AstNode* right = stack.pop();
            AstNode* left  = stack.pop();

            AstNode* op = pool.create(AstNode::Type::Operator, t.text);
            op->children = { left, right };

            stack.push(op);
            continue;
        }

        // --- Assignment ---
        if (t.type == TokenType::Assign) {
            if (stack.size() < 2)
                return fail("Hibás assignment: túl kevés operandus");

            AstNode* expr = stack.pop();
            AstNode* var  = stack.pop();

            AstNode* node = pool.create(AstNode::Type::Assignment, var->value);
            node->children.append(expr);
            stack.push(node);
            continue;
        }

        // --- Return ---
        if (t.type == TokenType::Return) {
            QVector<AstNode*> assignments;

            while (!stack.isEmpty() && stack.top()->type == AstNode::Type::Assignment)
                assignments.prepend(stack.pop());

            AstNode* node = pool.create(AstNode::Type::Return, "return");
            node->children = assignments;
            stack.push(node);
            continue;
        }

        // --- Statement vége ---
        if (t.type == TokenType::StatementEnd) {
            if (stack.isEmpty())
                return fail("Üres statement");

            AstNode* expr = stack.pop();
            AstNode* stmt = pool.create(AstNode::Type::Statement, "");
            stmt->children.append(expr);
            statements.append(stmt);
            continue;
        }

        // End / Unknown → ignoráljuk
    }

    // --- Végső ellenőrzések ---
    if (stack.isEmpty())
        return fail("Üres AST");

    if (!statements.isEmpty()) {
        AstNode* expr = stack.pop();
        AstNode* stmt = pool.create(AstNode::Type::Statement, "");
        stmt->children.append(expr);
        statements.append(stmt);

        if (statements.size() == 1)
            return Result<AstNode*>::success(statements[0]);

        AstNode* seq = pool.create(AstNode::Type::Sequence, "");
        seq->children = statements;
        return Result<AstNode*>::success(seq);
    }

    return Result<AstNode*>::success(stack.pop());
}
