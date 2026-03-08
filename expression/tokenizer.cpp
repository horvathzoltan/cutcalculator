#include "tokenizer.h"

Result<QVector<Token>> Tokenizer::tokenize(const QString& input)
{
    QVector<Token> tokens;
    int i = 0;
    const int n = input.size();
    int parenDepth = 0; // zárójel-mélység követése

    auto peek = [&](int offset = 0) -> QChar {
        return (i + offset < n) ? input[i + offset] : QChar{};
    };

    auto advance = [&]() -> QChar {
        return (i < n) ? input[i++] : QChar{};
    };

    while (i < n) {
        QChar c = peek();

        if (c == '\n') {
            tokens.append({TokenType::Newline, "\\n", 0, parenDepth});
            advance();
            continue;
        }

        if (c.isSpace()) {
            advance();
            continue;
        }

        // String literal: "...."
        if (c == '"') {
            advance(); // opening quote
            int start = i;
            while (i < n && peek() != '"') {
                advance();
            }

            if (i >= n) {
                return Result<QVector<Token>>::failure("Lezáratlan string literal");
            }

            QString text = input.mid(start, i - start);
            //if (peek() == '"')
            advance(); // closing quote
            tokens.append({TokenType::StringLiteral, text, 0, parenDepth});
            continue;
        }

        if (c.isDigit() || (c == '.' && peek(1).isDigit())) {

            int start = i;
            bool hasDot = false;
            while (i < n) {
                QChar d = peek();
                if (d == '.') {
                    if (hasDot) break;
                    hasDot = true;
                    advance();
                } else if (d.isDigit()) {
                    advance();
                } else {
                    break;
                }
            }
            tokens.append({TokenType::Number, input.mid(start, i - start), 0, parenDepth});
            continue;
        }

        if (c == '$') {
            advance();
            int start = i;
            while (i < n && (peek().isLetterOrNumber() || peek() == '_')) {
                advance();
            }
            tokens.append({TokenType::Variable, input.mid(start, i - start), 0, parenDepth});
            continue;
        }

        // --- Identifier / DSL keyword / Variable / Function ---
        if (c.isLetter() || c == '_') {
            int start = i;
            while (i < n && (peek().isLetterOrNumber() || peek() == '_')) {
                advance();
            }
            QString ident = input.mid(start, i - start);

            // DSL kulcsszavak
            // if (ident == "choose") {
            //     // Ha utána kettőspont jön, nyeljük be azt is: "choose:"
            //     if (peek() == ':') {
            //         advance(); // fogyasszuk el a ':'
            //         tokens.append({TokenType::Choose, "choose:"});
            //     } else {
            //         tokens.append({TokenType::Choose, ident});
            //     }
            //     continue;
            // }

            // if (ident == "opt") {
            //     // Ha utána kettőspont jön, nyeljük be azt is: "opt:"
            //     if (peek() == ':') {
            //         advance(); // fogyasszuk el a ':'
            //         tokens.append({TokenType::Opt, "opt:"});
            //     } else {
            //         tokens.append({TokenType::Opt, ident});
            //     }
            //     continue;
            // }


            // Ha az előző token Opt volt → a következő azonosító mindig flag (Variable)
            // if (!tokens.isEmpty() && tokens.last().type == TokenType::Opt) {
            //     tokens.append({TokenType::Variable, ident});
            //     continue;
            // }

            // Függvény: ha utána '(' jön
            if (peek() == '(') {
                tokens.append({TokenType::Function, ident, 0, parenDepth});
                continue;
            }

            if (ident == "return") {
                tokens.append({TokenType::Return, ident, 0, parenDepth});
                continue;
            }

            if (!ident.isEmpty() && ident[0].isUpper()) {
                tokens.append({TokenType::StringLiteral, ident, 0, parenDepth});
                continue;
            }

            // Minden más → Variable
            tokens.append({TokenType::Variable, ident, 0, parenDepth});
            continue;
        }


        // --- Relációs operátorok ---
        if (c == '>' && peek(1) == '=') {
            tokens.append({TokenType::GreaterEqual, ">=", 0, parenDepth});
            advance(); advance();
            continue;
        }

        if (c == '<' && peek(1) == '=') {
            tokens.append({TokenType::LessEqual, "<=", 0, parenDepth});
            advance(); advance();
            continue;
        }

        if (c == '=' && peek(1) == '=') {
            tokens.append({TokenType::Equal, "==", 0, parenDepth});
            advance(); advance();
            continue;
        }

        if (c == '>') {
            tokens.append({TokenType::Greater, ">", 0, parenDepth});
            advance();
            continue;
        }

        if (c == '<') {
            tokens.append({TokenType::Less, "<", 0, parenDepth});
            advance();
            continue;
        }

        switch (c.unicode()) {
        case '+': tokens.append({TokenType::Plus, QString(c), 0, parenDepth});    advance(); break;
        case '-': tokens.append({TokenType::Minus, QString(c), 0, parenDepth});   advance(); break;
        case '*': tokens.append({TokenType::Star, QString(c), 0, parenDepth});    advance(); break;
        case '/': tokens.append({TokenType::Slash, QString(c), 0, parenDepth});   advance(); break;
        case '(': tokens.append({TokenType::LParen, QString(c), 0, parenDepth}); parenDepth++; advance(); break;
        case ')': parenDepth--; tokens.append({TokenType::RParen, QString(c), 0, parenDepth}); advance(); break;
        case ',': tokens.append({TokenType::Comma, QString(c), 0, parenDepth});   advance(); break;
        case ':': tokens.append({TokenType::Colon, QString(c), 0, parenDepth});   advance(); break;
        case '?':
            if (peek(1) == '?') {
                // optional: "??"
                advance(); // első '?'
                advance(); // második '?'
                tokens.append({TokenType::OptionalQuestion, "??", 0, parenDepth});
            } else {
                // ternary: "?"
                advance();
                tokens.append({TokenType::TernaryQuestion, "?", 0, parenDepth});
            }
            break;
        case '=': tokens.append({TokenType::Assign, "=", 0, parenDepth}); advance(); break;

        default:
            return Result<QVector<Token>>::failure(
                QString("Ismeretlen karakter: '%1'").arg(c)
                );
            // tokens.append({TokenType::Unknown, QString(c)});
            // advance();
            // break;
        }
    }

    tokens.append({TokenType::End, {}, 0, parenDepth});
    return Result<QVector<Token>>::success(tokens);
}
