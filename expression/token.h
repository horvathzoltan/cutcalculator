#pragma once
#include <QString>

enum class TokenType {
    // --- Literálok ---
    Number,
    StringLiteral,

    // --- Azonosítók ---
    Variable,      // w, h, qty, x, base, margin, stb.
    Function,      // avg, min, max, stb.
    Identifier,    // (jelenleg nem használt; későbbi opt-flag / speciális azonosítók számára fenntartva)

    // --- DSL kulcsszavak ---
    Choose,        // choose
    Opt,           // opt

    // --- Operátorok ---
    Plus,
    Minus,

    PrefixPlus,
    PrefixMinus,

    Star,
    Slash,

    Greater,
    Less,
    GreaterEqual,
    LessEqual,
    Equal,

    Question,      // ?
    Colon,         // :
    Comma,         // ,
    LParen,        // (
    RParen,        // )

    Assign,      // '='
    Return,      // 'return'

    Newline,
    StatementEnd,
    End,
    Unknown


};

struct Token {
    TokenType type;
    QString   text;
    int       argc = 0;   // ÚJ: függvény argumentumszáma
};

