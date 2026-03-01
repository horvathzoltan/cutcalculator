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


QVector<Token> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token>  opStack;
    QStack<int>    argCountStack;

    // Ternary állapot: csak a klasszikus cond ? A : B → choose
    QStack<bool> questionHasColon;

    bool expectOperand = true;
    bool optMode       = false;   // épp egy opt: flag ? expr belsejében vagyunk

    int i = 0;
    const int n = tokens.size();

    auto emitChoose = [&]() {
        Token chooseTok;
        chooseTok.type = TokenType::Choose;
        chooseTok.text = "choose";
        output.append(chooseTok);
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
            emitChoose();
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
            if (top.type == TokenType::Question)
                emitChoose();
            else
                output.append(top);
        }
    };

    // OPT lezárása: flag ? expr → flag expr opt
    auto closeOptIfPending = [&]() {
        if (!optMode)
            return;

        // keressük a legutóbbi '?'-t
        int idx = opStack.size() - 1;
        while (idx >= 0 && opStack[idx].type != TokenType::Question)
            --idx;

        if (idx < 0)
            return;

        // a '?' fölötti operátorokat kiírjuk
        while (opStack.size() - 1 > idx)
            output.append(opStack.pop());

        // levesszük a '?'-t
        if (!opStack.isEmpty() && opStack.top().type == TokenType::Question)
            opStack.pop();

        // Itt az output stack tetején:
        //   ... flag expr
        // és most tesszük rá az opt‑ot postfixként
        Token optTok;
        optTok.type = TokenType::Opt;
        optTok.text = "opt";
        output.append(optTok);

        optMode = false;
    };

    while (i < n) {
        Token t = tokens[i++];

        // END
        if (t.type == TokenType::End) {
            closeOptIfPending();
            break;
        }

        // NEWLINE
        if (t.type == TokenType::Newline) {
            closeOptIfPending();
            flushAll();
            output.append({TokenType::StatementEnd, ";"});
            expectOperand = true;
            continue;
        }

        // LITERÁLOK / VÁLTOZÓK / STRINGEK
        if (t.type == TokenType::Number ||
            t.type == TokenType::Variable ||
            t.type == TokenType::StringLiteral)
        {
            output.append(t);
            expectOperand = false;
            continue;
        }

        // OPT PREFIX: "opt:" – csak módot kapcsolunk
        if (t.type == TokenType::Opt) {
            optMode = true;
            // maga az Opt token NEM kerül stackre, postfixként tesszük majd ki
            continue;
        }

        // TERNARY '?'
        if (t.type == TokenType::Question) {
            opStack.push(t);
            questionHasColon.push(false);
            expectOperand = true;
            continue;
        }

        // BAL ZÁRÓJEL
        if (t.type == TokenType::LParen) {
            opStack.push(t);
            expectOperand = true;
            continue;
        }

        // JOBB ZÁRÓJEL
        if (t.type == TokenType::RParen) {
            //closeOptIfPending();
            flushUntil(TokenType::LParen);
            if (!opStack.isEmpty() && opStack.top().type == TokenType::LParen)
                opStack.pop();

            // függvény lezárása
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

        // FÜGGVÉNY ARGUMENTUM VESSZŐ
        if (t.type == TokenType::Comma) {
            closeOptIfPending();
            flushUntil(TokenType::LParen);
            if (!argCountStack.isEmpty())
                argCountStack.top()++;
            closeAllTernaries();
            expectOperand = true;
            continue;
        }

        // COLON – TERNARY VAGY OPT
        if (t.type == TokenType::Colon) {

            // OPT‑mód: opt: flag ? expr
            if (optMode) {
                // az expr végét jelzi – kiértékeljük a '?' utáni kifejezést
                flushUntil(TokenType::Question);

                // a '?'‑t levesszük, de NEM choose‑t generálunk,
                // hanem majd closeOptIfPending() teszi rá az Opt‑ot
                if (!opStack.isEmpty() && opStack.top().type == TokenType::Question)
                    opStack.pop();

                // Itt az output tetején: ... flag expr
                Token optTok;
                optTok.type = TokenType::Opt;
                optTok.text = "opt";
                output.append(optTok);

                optMode = false;
                expectOperand = false;
                continue;
            }

            // CHOOSE‑mód: klasszikus cond ? A : B
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

        // SPECIAL CASE: +Number mint pozitív literál, ha operandust várunk
        if (t.type == TokenType::Plus && expectOperand && i < n && tokens[i].type == TokenType::Number) {
            output.append(tokens[i]);  // Number
            ++i;
            expectOperand = false;
            continue;
        }

        // SPECIAL CASE: unary '+' zárójel előtt: +(w/10) → (w/10)
        if (t.type == TokenType::Plus && expectOperand && i < n && tokens[i].type == TokenType::LParen) {
            // a '+' itt no‑op, egyszerűen eldobjuk
            continue;
        }

        // OPERÁTOROK
        if (isOperator(t)) {

            // PREFIX / INFIX felismerés a + és - operátorokra
            if (t.type == TokenType::Plus || t.type == TokenType::Minus) {

                if (expectOperand) {
                    // PREFIX
                    t.type = (t.type == TokenType::Plus)
                                 ? TokenType::PrefixPlus
                                 : TokenType::PrefixMinus;

                    opStack.push(t);
                    expectOperand = true;
                    continue;
                }
                // különben INFIX – esünk tovább az általános logikára
            }

            const OperatorInfo* info = getOpInfo(t);

            if (!info) {
                opStack.push(t);
                expectOperand = true;
                continue;
            }

            if (info->fixity == Fixity::Prefix) {

                if (t.type == TokenType::Choose)
                    continue;

                if (t.type == TokenType::Function) {
                    opStack.push(t);
                    argCountStack.push(0);
                    expectOperand = true;
                    continue;
                }

                if (t.type == TokenType::Return) {
                    opStack.push(t);
                    expectOperand = true;
                    continue;
                }

                opStack.push(t);
                expectOperand = true;
                continue;
            }

            // INFIX
            if (info->fixity == Fixity::Infix) {
                //closeOptIfPending();

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

        // minden más ignorálva
    }

    closeOptIfPending();
    flushAll();
    return output;
}
