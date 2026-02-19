#include "parser.h"
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
        return 0;   // legalacsonyabb precedencia

    case TokenType::Question:
    case TokenType::Colon:
        return 1;

    case TokenType::Plus:
    case TokenType::Minus:
        return 2;

    case TokenType::Star:
    case TokenType::Slash:
        return 3;

    case TokenType::Greater:
    case TokenType::Less:
    case TokenType::GreaterEqual:
    case TokenType::LessEqual:
    case TokenType::Equal:
        return 4;

    default:
        return 0;
    }
}


QVector<Token> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token> opStack;

    int i = 0;
    const int n = tokens.size();

    while (i < n) {
        Token t = tokens[i++];

        if (t.type == TokenType::End) {
            break;
        }

        // --- Literálok, változók ---
        if (t.type == TokenType::Number ||
            t.type == TokenType::Variable)
        {
            output.append(t);
            continue;
        }

        // --- Függvény név ---
        if (t.type == TokenType::Function) {
            opStack.push(t);
            continue;
        }

        // --- Vessző: függvény argumentum elválasztó ---
        if (t.type == TokenType::Comma) {
            while (!opStack.isEmpty() && opStack.top().type != TokenType::LParen) {
                output.append(opStack.pop());
            }
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
                opStack.pop(); // '('

            // Ha függvény áll a zárójel előtt, azt is kitesszük
            if (!opStack.isEmpty() && opStack.top().type == TokenType::Function) {
                output.append(opStack.pop());
            }
            continue;
        }

        // --- Ternary ? ---
        if (t.type == TokenType::Question) {
            opStack.push(t);
            continue;
        }

        // --- Ternary : ---
        if (t.type == TokenType::Colon) {
            while (!opStack.isEmpty() &&
                   opStack.top().type != TokenType::Question)
            {
                output.append(opStack.pop());
            }
            // ':' nem kerül a stackre
            continue;
        }


        // --- Bináris operátorok ---
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

                if (top.type == TokenType::LParen ||
                    top.type == TokenType::Function)
                    break;

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
        output.append(opStack.pop());
    }

    return output;
}
