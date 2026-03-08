#include "operator_info.h"
#include "expression/token.h"

#include <QHash>

static QHash<TokenType, OperatorInfo> OPINFO = {

// Prefix operátorok
    // { TokenType::Opt,        { 5, false, Fixity::Prefix, 2 } },
    // { TokenType::Choose,     { 5, false, Fixity::Prefix, 3 } },
    { TokenType::Return,     { 5, false, Fixity::Prefix, 1 } },
    { TokenType::Function,   { 5, false, Fixity::Prefix, -1 } }, // -1 = variadic

    // Infix bináris operátorok
    { TokenType::Star,       { 4, false, Fixity::Infix, 2 } },
    { TokenType::Slash,      { 4, false, Fixity::Infix, 2 } },

    { TokenType::Plus,       { 3, false, Fixity::Infix, 2 } },
    { TokenType::Minus,      { 3, false, Fixity::Infix, 2 } },

    { TokenType::PrefixPlus,  { 5, false, Fixity::Prefix, 1 } },
    { TokenType::PrefixMinus, { 5, false, Fixity::Prefix, 1 } },

    { TokenType::Greater,        { 2, false, Fixity::Infix, 2 } },
    { TokenType::Less,           { 2, false, Fixity::Infix, 2 } },
    { TokenType::GreaterEqual,   { 2, false, Fixity::Infix, 2 } },
    { TokenType::LessEqual,      { 2, false, Fixity::Infix, 2 } },
    { TokenType::Equal,          { 2, false, Fixity::Infix, 2 } },

    { TokenType::Assign,     { 1, true,  Fixity::Infix, 2 } }, // jobbról asszociatív

    // Optional infix operátor: flag ?? expr
    { TokenType::OptionalQuestion, { 2, false, Fixity::Infix, 2 } },

    // Ternary operátor
    { TokenType::TernaryQuestion,   { 0, true,  Fixity::Ternary, 3 } },


// A ':' nem operátor, csak a ternary része → külön kezeljük a parserben
};

bool isOperator(const Token& t) {
    return OPINFO.contains(t.type);
}

const OperatorInfo* getOpInfo(const Token& t) {
    if (!OPINFO.contains(t.type))
        return nullptr;
    return &OPINFO[t.type];
}
