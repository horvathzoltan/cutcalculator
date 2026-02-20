#include "tokenizer.h"

QVector<Token> Tokenizer::tokenize(const QString& input)
{
    QVector<Token> tokens;
    int i = 0;
    const int n = input.size();

    auto peek = [&](int offset = 0) -> QChar {
        return (i + offset < n) ? input[i + offset] : QChar{};
    };

    auto advance = [&]() -> QChar {
        return (i < n) ? input[i++] : QChar{};
    };

    while (i < n) {
        QChar c = peek();

        if (c == '\n') {
            tokens.append({TokenType::Newline, "\\n"});
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
            QString text = input.mid(start, i - start);
            if (peek() == '"')
                advance(); // closing quote
            tokens.append({TokenType::StringLiteral, text});
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
            tokens.append({TokenType::Number, input.mid(start, i - start)});
            continue;
        }

        if (c == '$') {
            advance();
            int start = i;
            while (i < n && (peek().isLetterOrNumber() || peek() == '_')) {
                advance();
            }
            tokens.append({TokenType::Variable, input.mid(start, i - start)});
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
            if (ident == "choose") {
                tokens.append({TokenType::Choose, ident});
                continue;
            }
            if (ident == "opt") {
                tokens.append({TokenType::Opt, ident});
                continue;
            }

            // Ha az előző token Opt volt → a következő azonosító mindig flag (Variable)
            if (!tokens.isEmpty() && tokens.last().type == TokenType::Opt) {
                tokens.append({TokenType::Variable, ident});
                continue;
            }

            // Függvény: ha utána '(' jön
            if (peek() == '(') {
                tokens.append({TokenType::Function, ident});
                continue;
            }

            if (ident == "return") {
                tokens.append({TokenType::Return, ident});
                continue;
            }

            if (!ident.isEmpty() && ident[0].isUpper()) {
                tokens.append({TokenType::StringLiteral, ident});
                continue;
            }

            // Minden más → Variable
            tokens.append({TokenType::Variable, ident});
            continue;
        }


        // --- Relációs operátorok ---
        if (c == '>' && peek(1) == '=') {
            tokens.append({TokenType::GreaterEqual, ">="});
            advance(); advance();
            continue;
        }

        if (c == '<' && peek(1) == '=') {
            tokens.append({TokenType::LessEqual, "<="});
            advance(); advance();
            continue;
        }

        if (c == '=' && peek(1) == '=') {
            tokens.append({TokenType::Equal, "=="});
            advance(); advance();
            continue;
        }

        if (c == '>') {
            tokens.append({TokenType::Greater, ">"});
            advance();
            continue;
        }

        if (c == '<') {
            tokens.append({TokenType::Less, "<"});
            advance();
            continue;
        }

        switch (c.unicode()) {
        case '+': tokens.append({TokenType::Plus, QString(c)});    advance(); break;
        case '-': tokens.append({TokenType::Minus, QString(c)});   advance(); break;
        case '*': tokens.append({TokenType::Star, QString(c)});    advance(); break;
        case '/': tokens.append({TokenType::Slash, QString(c)});   advance(); break;
        case '(': tokens.append({TokenType::LParen, QString(c)});  advance(); break;
        case ')': tokens.append({TokenType::RParen, QString(c)});  advance(); break;
        case ',': tokens.append({TokenType::Comma, QString(c)});   advance(); break;
        case ':': tokens.append({TokenType::Colon, QString(c)});   advance(); break;
        case '?': tokens.append({TokenType::Question, QString(c)});advance(); break;
        case '=':
            tokens.append({TokenType::Assign, "="});
            advance();
            break;

        default:
            tokens.append({TokenType::Unknown, QString(c)});
            advance();
            break;
        }
    }

    tokens.append({TokenType::End, {}});
    return tokens;
}
