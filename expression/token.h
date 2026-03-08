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
    TernaryOp,
    OptionalOp,

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

    TernaryQuestion,   // ?
    OptionalQuestion,  // ??
    //Question,      // ?
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
    TokenType type;       // token típusa
    QString   text;       // eredeti szöveg
    int       argc = 0;   // függvény argumentumszáma
    int       parenDepth = 0; // zárójel-mélység
};

