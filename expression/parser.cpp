#include "parser.h"
#include "common/logger/logger.h"
#include "operator_info.h"
#include "tokenizer.h"
#include <QStack>

Result<Parser::ParseResult> Parser::parse(const QString& input)
{
    Parser::ParseResult pr;

    // 1) Tokenizálás
    auto tokens = Tokenizer::tokenize(input);
    if (!tokens.ok) {
        return Result<ParseResult>::failure(
            QString("Tokenizálási hiba: %1").arg(tokens.error)
            );
    }
    pr.tokens = tokens.value;

    // 2) RPN konverzió
    auto rpn = toRpn(pr.tokens);
    if (!rpn.ok) {
        return Result<ParseResult>::failure(
            QString("RPN hiba: %1").arg(rpn.error)
            );
    }
    pr.rpn = rpn.value;

    // 3) Siker
    return Result<ParseResult>::success(pr);
}


Result<QVector<Token>> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token>  opStack;
    QStack<int>    argCountStack;
    QStack<bool>   questionHasColon;

    bool expectOperand = true;

    int i = 0;
    const int n = tokens.size();

    auto fail = [&](const QString& msg) {
        return Result<QVector<Token>>::failure(msg);
    };

    auto flushUntil = [&](TokenType stop) {
        while (!opStack.isEmpty() && opStack.top().type != stop) {
            output.append(opStack.pop());
        }
    };

    auto closeOneTernaryIfPossible = [&]() {
        int idx = opStack.size() - 1;
        while (idx >= 0 && opStack[idx].type != TokenType::Question)
            --idx;

        if (idx < 0)
            return;

        if (questionHasColon.isEmpty() || !questionHasColon.top())
            return;

        while (opStack.size() - 1 > idx)
            output.append(opStack.pop());

        if (!opStack.isEmpty() && opStack.top().type == TokenType::Question) {
            opStack.pop();
            questionHasColon.pop();
            output.append({TokenType::TernaryOp, "?:", 2});
        }
    };

    auto closeAllTernaries = [&]() {
        while (true) {
            int idx = opStack.size() - 1;
            while (idx >= 0 && opStack[idx].type != TokenType::Question)
                --idx;

            if (idx < 0)
                break;

            if (questionHasColon.isEmpty() || !questionHasColon.top())
                break;

            closeOneTernaryIfPossible();
        }
    };

    auto flushAll = [&]() {
        closeAllTernaries();
        while (!opStack.isEmpty()) {
            Token top = opStack.pop();
            if (top.type == TokenType::Question){
                // optional
                if (!questionHasColon.isEmpty())
                    questionHasColon.pop();
                output.append({TokenType::OptionalOp, "opt", 1});
                continue;
            }
            else if (top.type == TokenType::LParen)
                return fail("Hiányzó ')'");
            else
                output.append(top);
        }

        return Result<QVector<Token>>::success(output);
    };

    while (i < n) {
        Token t = tokens[i++];

        if (t.type == TokenType::End) {
            break;
        }

        if (t.type == TokenType::Newline) {
            auto r = flushAll();
            if (!r.ok) return r;
            output.append({TokenType::StatementEnd, ";"});
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::Number ||
            t.type == TokenType::Variable ||
            t.type == TokenType::StringLiteral)
        {
            output.append(t);
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::Question) {
            opStack.push(t);
            questionHasColon.push(false);
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::LParen) {
            opStack.push(t);
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::RParen) {

            auto hasLParen = [&]() {
                for (const Token& tk : opStack)
                    if (tk.type == TokenType::LParen)
                        return true;
                return false;
            };

            if (!hasLParen())
                return fail("Váratlan ')'");

            flushUntil(TokenType::LParen);

            if (opStack.isEmpty())
                return fail("Váratlan ')'");

            opStack.pop(); // LParen

            if (!opStack.isEmpty() && opStack.top().type == TokenType::Function) {
                Token fn = opStack.pop();
                int argc = 1;
                if (!argCountStack.isEmpty())
                    argc += argCountStack.pop();
                fn.argc = argc;
                output.append(fn);
            }

            closeAllTernaries();
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::Comma) {
            flushUntil(TokenType::LParen);
            if (!argCountStack.isEmpty())
                argCountStack.top()++;
            closeAllTernaries();
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::Colon) {
            auto hasQuestion = [&]() {
                for (const Token& tk : opStack)
                    if (tk.type == TokenType::Question)
                        return true;
                return false;
            };

            if (!hasQuestion())
                return fail("':' csak ternary operátor részeként használható");

            while (!opStack.isEmpty() && opStack.top().type != TokenType::Question)
                output.append(opStack.pop());

            if (!questionHasColon.isEmpty()) {
                if (!questionHasColon.top())
                    questionHasColon.top() = true;
                else
                    closeOneTernaryIfPossible();
            }

            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::Plus && expectOperand && i < n && tokens[i].type == TokenType::Number) {
            output.append(tokens[i]);
            ++i;
            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::Plus && expectOperand && i < n && tokens[i].type == TokenType::LParen) {
            continue;
        }

        if (isOperator(t)) {

            if (t.type == TokenType::Plus || t.type == TokenType::Minus) {
                if (expectOperand) {
                    t.type = (t.type == TokenType::Plus)
                    ? TokenType::PrefixPlus
                    : TokenType::PrefixMinus;

                    opStack.push(t);
                    expectOperand = true;
                    continue;
                }
            }

            const OperatorInfo* info = getOpInfo(t);

            if (!info) {
                opStack.push(t);
                expectOperand = true;
                continue;
            }

            if (info->fixity == Fixity::Prefix) {
                opStack.push(t);
                expectOperand = true;
                continue;
            }

            if (info->fixity == Fixity::Infix) {

                while (!opStack.isEmpty()) {
                    const Token& top = opStack.top();
                    const OperatorInfo* topInfo = getOpInfo(top);
                    if (!topInfo)
                        break;

                    if (top.type == TokenType::LParen ||
                        top.type == TokenType::Question)
                        break;

                    bool shouldPop =
                        (!info->rightAssociative && topInfo->precedence >= info->precedence) ||
                        (info->rightAssociative && topInfo->precedence > info->precedence);

                    if (!shouldPop)
                        break;

                    output.append(opStack.pop());
                }

                opStack.push(t);
                expectOperand = true;
                continue;
            }

            continue;
        }

        return fail(QString("Váratlan token: %1").arg(t.text));
    }

    auto r = flushAll();
    if (!r.ok) return r;

    return Result<QVector<Token>>::success(output);
}
