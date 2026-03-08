#include "parser.h"
#include "common/logger/logger.h"
#include "expression/ast_printer.h"
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

    if(tokens.value.isEmpty()) {
        return Result<ParseResult>::failure(
            QString("Nincsenek tokenek")
            );
    }

    pr.tokens = tokens.value;

    // 2) RPN konverzió
    auto rpn = toRpn(pr.tokens);
    if (!rpn.ok) {
        pr.rpn = rpn.value; // részleges RPN is jöhet
        return Result<ParseResult>::failureWithValue(
            pr,
            QString("RPN hiba: %1").arg(rpn.error)
            );
    }

    pr.rpn = rpn.value;

    // 3) Siker
    return Result<ParseResult>::success(pr);
}

void Parser::debugDump(const ParseResult& pr, const QString& code, const QString& error)
{
    zInfo() << "\n================ PARSER DEBUG ================";
    zInfo() << "Code:" << code;

    // ERROR
    if (!error.isEmpty()) {
        zInfo() << "\n=== ERROR ===";
        zInfo() << error;
    }

    // TOKENS
    zInfo() << "\n=== Tokens ===";
    if (!pr.tokens.isEmpty()) {
        for (const Token& t : pr.tokens) {
            zInfo() << " "
                    << tokenTypeName(t.type)
                    << "('" << t.text << "')"
                    << " argc=" << t.argc
                    << " depth=" << t.parenDepth;
        }
    } else {
        zInfo() << " (no tokens)";
    }

    // RPN
    zInfo() << "\n=== RPN ===";
    if (!pr.rpn.isEmpty()) {
        for (const Token& t : pr.rpn) {
            zInfo() << " "
                    << t.text
                    << " (" << tokenTypeName(t.type)
                    << ", argc=" << t.argc
                    << ", depth=" << t.parenDepth
                    << ")";
        }
    } else {
        zInfo() << " (no RPN)";
    }

    zInfo() << "==============================================\n";
}


Result<QVector<Token>> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token>  opStack;
    QStack<int>    argCountStack;
    QStack<bool>   ternaryHasColon;   // csak TernaryQuestion-ökhöz

    bool expectOperand = true;

    int i = 0;
    const int n = tokens.size();
    int parenDepth = 0; // csak debug / trace célra

    auto fail = [&](const QString& msg) {
        return Result<QVector<Token>>::failure(msg);
    };

    // Operátorok kipakolása egy adott tokenig (LParen-ig), de TernaryQuestion-ön megállunk
    auto flushUntil = [&](TokenType stop) {
        while (!opStack.isEmpty()) {
            TokenType tt = opStack.top().type;
            if (tt == stop || tt == TokenType::TernaryQuestion)
                break;
            output.append(opStack.pop());
        }
    };

    // Minden operátor kipakolása a stackből, a még nyitott ternary-k lezárásával
    auto flushAll = [&]() -> Result<QVector<Token>> {
        while (!opStack.isEmpty()) {
            Token top = opStack.pop();

            if (top.type == TokenType::LParen)
                return fail("Hiányzó ')'");

            if (top.type == TokenType::OptionalQuestion) {
                output.append({TokenType::OptionalOp, "opt", 2});
                continue;
            }
            if (top.type == TokenType::TernaryQuestion) {
                if (ternaryHasColon.isEmpty() || !ternaryHasColon.top())
                    return fail("Hiányzó ':' a ternary operátorhoz");
                ternaryHasColon.pop();
                output.append({TokenType::TernaryOp, "?:", 2});
                continue;
            }

            output.append(top);
        }

        if (!ternaryHasColon.isEmpty())
            return fail("Hiányzó ':' a ternary operátorhoz");

        return Result<QVector<Token>>::success(output);
    };

    while (i < n) {
        Token t = tokens[i++];
        t.parenDepth = parenDepth;

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

        // Ternary kezdete: cond ? trueExpr : falseExpr
        if (t.type == TokenType::TernaryQuestion) {
            opStack.push(t);
            ternaryHasColon.push(false);
            expectOperand = true;
            continue;
        }

        // Optional: flag ?? expr  → RPN: flag expr OptionalOp
        if (t.type == TokenType::OptionalQuestion) {
            // kezeljük, mint normál infix operátort (precedencia az OPINFO-ban)
            const OperatorInfo* info = getOpInfo(t);
            if (!info) {
                opStack.push(t);
                expectOperand = true;
                continue;
            }

            while (!opStack.isEmpty()) {
                const Token& top = opStack.top();
                const OperatorInfo* topInfo = getOpInfo(top);
                if (!topInfo)
                    break;

                if (top.type == TokenType::LParen ||
                    top.type == TokenType::TernaryQuestion)
                    break;

                bool shouldPop =
                    (!info->rightAssociative && topInfo->precedence >= info->precedence) ||
                    (info->rightAssociative && topInfo->precedence > info->precedence);

                if (!shouldPop)
                    break;

                Token popped = opStack.pop();
                if (popped.type == TokenType::OptionalQuestion)
                    output.append({TokenType::OptionalOp, "opt", 2});
                else
                    output.append(popped);

            }

            opStack.push(t);
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::LParen) {
            t.parenDepth = parenDepth;
            opStack.push(t);
            parenDepth++;
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::RParen) {
            parenDepth--;
            t.parenDepth = parenDepth;

            auto hasLParen = [&]() {
                for (const Token& tk : opStack)
                    if (tk.type == TokenType::LParen)
                        return true;
                return false;
            };

            if (!hasLParen())
                return fail("Váratlan ')'");

            // Flush az LParen-ig: operátorokat kipakoljuk,
            // TernaryQuestion-t hagyjuk a helyén (nem pakoljuk ki, nem állunk meg)
            while (!opStack.isEmpty() && opStack.top().type != TokenType::LParen) {
                Token top = opStack.top();
                if (top.type == TokenType::TernaryQuestion) {
                    // Ha van hozzá ':' → lezárjuk
                    if (!ternaryHasColon.isEmpty() && ternaryHasColon.top()) {
                        opStack.pop();
                        ternaryHasColon.pop();
                        output.append({TokenType::TernaryOp, "?:", 2});
                    } else {
                        // Nincs még ':' → hagyjuk, de nem tudunk továbblépni
                        // Ez hiba lenne (pl. '(cond ?' zárójel nélkül a ':' előtt)
                        break;
                    }
                } else {
                    output.append(opStack.pop());
                }
            }

            if (opStack.isEmpty() || opStack.top().type != TokenType::LParen)
                return fail("Váratlan ')'");
            opStack.pop(); // biztosan LParen

            // Function kezelés
            if (!opStack.isEmpty() && opStack.top().type == TokenType::Function) {
                Token fn = opStack.pop();
                int argc = 1;
                if (!argCountStack.isEmpty())
                    argc += argCountStack.pop();
                fn.argc = argc;
                output.append(fn);
            }

            // Külső ternary lezárása ha szükséges (a zárójel a false-ágat zárta le)
            while (!opStack.isEmpty() &&
                   opStack.top().type == TokenType::TernaryQuestion &&
                   !ternaryHasColon.isEmpty() &&
                   ternaryHasColon.top())
            {
                opStack.pop();
                ternaryHasColon.pop();
                output.append({TokenType::TernaryOp, "?:", 2});
            }

            expectOperand = false;
            continue;
        }

        if (t.type == TokenType::Comma) {
            flushUntil(TokenType::LParen);
            if (!argCountStack.isEmpty())
                argCountStack.top()++;
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::Colon) {
            // keresd meg a legfelső TernaryQuestion-t
            int qIndex = -1;
            for (int k = opStack.size() - 1; k >= 0; --k) {
                if (opStack[k].type == TokenType::TernaryQuestion) {
                    qIndex = k;
                    break;
                }
            }

            if (qIndex < 0)
                return fail("':' csak ternary operátor részeként használható");

            // a '?' fölötti operátorokat kipakoljuk
            while (!opStack.isEmpty() && opStack.top().type != TokenType::TernaryQuestion)
                output.append(opStack.pop());

            if (ternaryHasColon.isEmpty())
                return fail("Belső hiba: ternaryHasColon stack üres");

            if (ternaryHasColon.top())
                return fail("Ugyanahhoz a ternary operátorhoz több ':' tartozik");

            ternaryHasColon.top() = true;

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
                        top.type == TokenType::TernaryQuestion)
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

/*
Result<QVector<Token>> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token>  opStack;
    QStack<int>    argCountStack;
    QStack<bool>   questionHasColon;

    bool expectOperand = true;

    int i = 0;
    const int n = tokens.size();
    int parenDepth = 0; // zárójel-mélység követése

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
        while (idx >= 0 && opStack[idx].type != TokenType::TernaryQuestion)
            --idx;

        if (idx < 0)
            return;

        if (questionHasColon.isEmpty() || !questionHasColon.top())
            return;

        while (opStack.size() - 1 > idx)
            output.append(opStack.pop());

        if (!opStack.isEmpty() && opStack.top().type == TokenType::TernaryQuestion) {
            opStack.pop();
            questionHasColon.pop();
            output.append({TokenType::TernaryOp, "?:", 2});
        }
    };

    auto closeAllTernaries = [&]() {
        while (true) {
            int idx = opStack.size() - 1;
            while (idx >= 0 && opStack[idx].type != TokenType::TernaryQuestion)
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
            if (top.type == TokenType::TernaryQuestion){
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
        t.parenDepth = parenDepth; // parser oldali zárójel-mélység

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

        if (t.type == TokenType::TernaryQuestion) {
            // nested ternary/optional csak zárójelben engedélyezett
            if (!questionHasColon.isEmpty() && parenDepth == 0) {
                return fail("Nested ternary/optional requires parentheses");
            }

            // optional true-ág: összetett kifejezés csak zárójelben engedélyezett
            if (i < n) {
                Token next = tokens[i];
                bool nextIsSimple =
                    next.type == TokenType::Variable ||
                    next.type == TokenType::Number ||
                    next.type == TokenType::StringLiteral;

                if (!nextIsSimple && next.type != TokenType::LParen) {
                    return fail("Optional expression requires parentheses for complex expressions");
                }
            }

            opStack.push(t);
            questionHasColon.push(false);
            expectOperand = true;
            continue;
        }


        if (t.type == TokenType::LParen) {
            t.parenDepth = parenDepth;
            opStack.push(t);
            parenDepth++;
            expectOperand = true;
            continue;
        }

        if (t.type == TokenType::RParen) {
            parenDepth--;
            t.parenDepth = parenDepth;

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

            // nested ternary lezárása, ha a zárójel véget ért
            // ha a zárójel egy ternary true/false ágát zárja le → zárjuk le a ternary-t
            {
                int qIndex = -1;
                for (int k = opStack.size() - 1; k >= 0; --k) {
                    if (opStack[k].type == TokenType::TernaryQuestion &&
                        opStack[k].parenDepth == parenDepth)
                    {
                        qIndex = k;
                        break;
                    }
                }

                if (qIndex >= 0) {
                    // 1) A '?' fölötti operátorokat kipakoljuk, DE a LParen-t NEM!
                    while (opStack.size() - 1 > qIndex) {
                        if (opStack.top().type != TokenType::LParen)
                            output.append(opStack.pop());
                        else
                            break;
                    }

                    // 2) Töröljük a '?' operátort a stackből
                    opStack.remove(qIndex);

                    // 3) Töröljük a hozzá tartozó colon-state-et
                    questionHasColon.pop();

                    // 4) Hozzáadjuk a ternary operátort
                    output.append({TokenType::TernaryOp, "?:", 2});
                }
            }



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

            // keresd meg a legfelső '?'-t
            int qIndex = -1;
            for (int k = opStack.size() - 1; k >= 0; --k) {
                if (opStack[k].type == TokenType::TernaryQuestion) {
                    qIndex = k;
                    break;
                }
            }

            if (qIndex < 0)
                return fail("':' csak ternary operátor részeként használható");

            // ugyanabban a zárójel-szintben kell lennie
            if (opStack[qIndex].parenDepth != parenDepth)
                return fail("Nested ternary requires parentheses");

            // a '?' fölötti operátorokat kipakoljuk
            while (!opStack.isEmpty() && opStack.top().type != TokenType::TernaryQuestion)
                output.append(opStack.pop());

            // jelöljük, hogy ehhez a '?'‑hez már volt ':'
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
                        top.type == TokenType::TernaryQuestion)
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
*/
