#pragma once
#include <QString>

enum class TokenType {
    Number,
    Identifier,
    Variable,
    Function,      // ÚJ: függvénynév (avg, sum, stb.)

    Plus,
    Minus,
    Star,
    Slash,
    LParen,
    RParen,
    Comma,
    Colon,
    Question,

    Greater,
    Less,
    GreaterEqual,
    LessEqual,
    Equal,

    End,
    Unknown
};

struct Token {
    TokenType type;
    QString   text;
};
