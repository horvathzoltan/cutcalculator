#include "parser.h"
#include "tokenizer.h"
#include <QStack>

int Parser::precedence(TokenType t)
{
    switch (t) {
    case TokenType::Star:
    case TokenType::Slash:
        return 3;
    case TokenType::Plus:
    case TokenType::Minus:
        return 2;
    case TokenType::Greater:
    case TokenType::Less:
    case TokenType::GreaterEqual:
    case TokenType::LessEqual:
    case TokenType::Equal:
        return 1;
    default:
        return 0;
    }
}

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

QVector<Token> Parser::toRpn(const QVector<Token>& tokens)
{
    QVector<Token> output;
    QStack<Token>  stack;

    for (int i = 0; i < tokens.size(); ++i) {
        const Token& tok = tokens[i];

        if (tok.type == TokenType::Number ||
            tok.type == TokenType::Variable) {
            output.append(tok);
            continue;
        }

        // Identifier → lehet függvény, ha utána '(' jön
        if (tok.type == TokenType::Identifier) {
            bool isFunc = false;
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::LParen) {
                isFunc = true;
            }

            if (isFunc) {
                Token funcTok{TokenType::Function, tok.text};
                stack.push(funcTok);
            } else {
                // ha sima azonosítóként akarod kezelni, mehet operandusnak
                output.append(tok);
            }
            continue;
        }

        if (tok.type == TokenType::Comma) {
            // argumentum elválasztó: pop-olunk, amíg LParen vagy stack üres
            while (!stack.isEmpty() && stack.top().type != TokenType::LParen) {
                output.append(stack.pop());
            }
            continue;
        }

        if (tok.type == TokenType::LParen) {
            stack.push(tok);
            continue;
        }

        if (tok.type == TokenType::RParen) {
            while (!stack.isEmpty() && stack.top().type != TokenType::LParen) {
                output.append(stack.pop());
            }
            if (!stack.isEmpty() && stack.top().type == TokenType::LParen) {
                stack.pop(); // '(' le
            }
            // ha a '(' előtt függvény volt, azt is kidobjuk az outputra
            if (!stack.isEmpty() && stack.top().type == TokenType::Function) {
                output.append(stack.pop());
            }
            continue;
        }

        if (isOperator(tok.type)) {
            while (!stack.isEmpty() && isOperator(stack.top().type)) {
                TokenType topType = stack.top().type;
                int p1 = precedence(tok.type);
                int p2 = precedence(topType);

                if ((isLeftAssociative(tok.type) && p1 <= p2) ||
                    (!isLeftAssociative(tok.type) && p1 < p2)) {
                    output.append(stack.pop());
                } else {
                    break;
                }
            }
            stack.push(tok);
            continue;
        }

        if (tok.type == TokenType::End) {
            break;
        }

        // Unknown stb.: most ignorálhatjuk vagy kidobhatjuk
    }

    while (!stack.isEmpty()) {
        output.append(stack.pop());
    }

    return output;
}
