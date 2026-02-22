#include "parser.h"
#include "common/logger/logger.h"
#include "tokenizer.h"
#include <QStack>

bool Parser::isLeftAssociative(TokenType t)
{
    // minden bináris operátor balról asszociatív most
    switch (t) {
    case TokenType::Plus:
    case TokenType::Minus:
    case TokenType::Star:
    case TokenType::Slash:
    case TokenType::Greater:
    case TokenType::Less:
    case TokenType::GreaterEqual:
    case TokenType::LessEqual:
    case TokenType::Equal:
        return true;
    default:
        return false;
    }
}

bool Parser::isOperator(TokenType t)
{
    switch (t) {
    case TokenType::Plus:
    case TokenType::Minus:
    case TokenType::Star:
    case TokenType::Slash:
    case TokenType::Greater:
    case TokenType::Less:
    case TokenType::GreaterEqual:
    case TokenType::LessEqual:
    case TokenType::Equal:
        return true;
    default:
        return false;
    }
}

QVector<Token> Parser::parseToRpn(const QString& input)
{
    auto tokens = Tokenizer::tokenize(input);
    return toRpn(tokens);
}

int Parser::precedence(const Token& t)
{
    switch (t.type) {
    case TokenType::Assign:
        return 0;

        // Ternary ? : már külön ágon kezelve, nem bináris operátor:
        // case TokenType::Question:
        // case TokenType::Colon:

    case TokenType::Greater:
    case TokenType::Less:
    case TokenType::GreaterEqual:
    case TokenType::LessEqual:
    case TokenType::Equal:
        return 2;

    case TokenType::Plus:
    case TokenType::Minus:
        return 3;

    case TokenType::Star:
    case TokenType::Slash:
        return 4;

    default:
        return 0;
    }
}

QVector<Token> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token> opStack;
    QStack<int> argCountStack;   // ÚJ: függvény argumentumszámláló

    int i = 0;
    const int n = tokens.size();

    while (i < n) {
        Token t = tokens[i++];

        if (t.type == TokenType::End) {
            break;
        }

        // --- Literálok, változók ---
        if (t.type == TokenType::Newline) {
            while (!opStack.isEmpty())
                output.append(opStack.pop());
            output.append({TokenType::StatementEnd, ";"});
            continue;
        }

        if (t.type == TokenType::Number ||
            t.type == TokenType::Variable ||
            t.type == TokenType::StringLiteral)   // <-- EZ HIÁNYZOTT
        {
            output.append(t);
            continue;
        }


        // Prefix choose: → megy a stackre (ternary-hez kapcsolódik)
        // Prefix opt: → NEM megy a stackre, azonnal outputba kerül
        if (t.type == TokenType::Choose) {
            opStack.push(t);
            continue;
        }

        if (t.type == TokenType::Opt) {
            // Az opt két operandust vár, prefix operátor → outputba tesszük
            output.append(t);
            continue;
        }



        // --- Függvény név ---
        if (t.type == TokenType::Function) {
            opStack.push(t);
            argCountStack.push(0);   // ÚJ: indul az argumentumszámlálás
            continue;
        }


        // --- Vessző: függvény argumentum elválasztó ---
        if (t.type == TokenType::Comma) {
            while (!opStack.isEmpty() && opStack.top().type != TokenType::LParen) {
                output.append(opStack.pop());
            }
            if (!argCountStack.isEmpty())
                argCountStack.top()++;   // ÚJ: új argumentum
            continue;
        }


        // --- Bal zárójel ---
        if (t.type == TokenType::LParen) {
            opStack.push(t);
            continue;
        }

        // --- Jobb zárójel ---
        if (t.type == TokenType::RParen) {
            while (!opStack.isEmpty() && opStack.top().type != TokenType::LParen) {
                output.append(opStack.pop());
            }

            if (!opStack.isEmpty() && opStack.top().type == TokenType::LParen)
                opStack.pop();

            // Függvény lezárása
            if (!opStack.isEmpty() && opStack.top().type == TokenType::Function) {
                Token fn = opStack.pop();

                int argc = 1;  // legalább 1 paraméter
                if (!argCountStack.isEmpty()) {
                    argc += argCountStack.pop();
                }

                fn.argc = argc;   // ÚJ: paraméterszám beállítása
                output.append(fn);
            }

            continue;
        }


        // Ternary: ? → push
        if (t.type == TokenType::Question) {
            opStack.push(t);
            continue;
        }

        // Ternary: ':' → kipucoljuk a bináris operátorokat a '?'-ig,
        // de a '?' a stacken marad, a ternary még nem zárul le.
        if (t.type == TokenType::Colon) {
            while (!opStack.isEmpty() && opStack.top().type != TokenType::Question) {
                output.append(opStack.pop());
            }
            continue;
        }



        // --- Bináris operátorok (és = ) ---
        // --- Bináris operátorok (és =) ---
        if (t.type == TokenType::Plus  ||
            t.type == TokenType::Minus ||
            t.type == TokenType::Star  ||
            t.type == TokenType::Slash ||
            t.type == TokenType::Greater ||
            t.type == TokenType::Less    ||
            t.type == TokenType::GreaterEqual ||
            t.type == TokenType::LessEqual    ||
            t.type == TokenType::Equal ||
            t.type == TokenType::Assign)
        {
            while (!opStack.isEmpty()) {
                Token top = opStack.top();

                // Zárójel / függvény megállítja
                if (top.type == TokenType::LParen ||
                    top.type == TokenType::Function)
                    break;

                // // --- TERNARY LEZÁRÁSA ---
                // // Ha '?' van a stacken → előbb zárjuk le a ternary-t
                // if (top.type == TokenType::Question) {
                //     opStack.pop(); // levesszük a '?'

                //     Token chooseTok;
                //     chooseTok.type = TokenType::Choose;

                //     // Prefix choose támogatása
                //     if (!opStack.isEmpty() && opStack.top().type == TokenType::Choose) {
                //         chooseTok.text = "choose:";
                //         opStack.pop();
                //     } else {
                //         chooseTok.text = "choose";
                //     }

                //     output.append(chooseTok);
                //     continue;
                // }

                // --- Normál bináris precedencia ---
                if (precedence(top) < precedence(t))
                    break;

                output.append(opStack.pop());
            }

            opStack.push(t);
            continue;
        }


        if (t.type == TokenType::Return) {
            // A return token egyszerűen bekerül az outputba a végén.
            // A return után jövő assignmentek RPN-be kerülnek normál módon.
            opStack.push(t);
            continue;
        }

        // Minden más: ignoráljuk
    }

    // A végén mindent kipakolunk
    while (!opStack.isEmpty()) {
        Token top = opStack.pop();

        // Ha '?' van a stacken → előbb zárjuk le a ternary-t
        if (top.type == TokenType::Question) {
            opStack.pop();
            Token chooseTok;
            chooseTok.type = TokenType::Choose;
            chooseTok.text = "choose";
            output.append(chooseTok);
            continue;
        }


        if (top.type == TokenType::Colon) {
            // ':' soha nem kerülhet az outputba
            continue;
        }

        // Prefix choose: / opt: csak jelölő volt → NE kerüljön az RPN-be
        if (top.type == TokenType::Choose || top.type == TokenType::Opt) {
            continue;
        }

        output.append(top);
    }




    // IDEIGLENES DEBUG:
    zInfo() << "RPN for choose test:";
    for (const auto& tok : output) {
        zInfo() << "  " << (int)tok.type << tok.text << "argc=" << tok.argc;
    }


    return output;
}
