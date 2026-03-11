#include "operator_info.h"
#include "expression/token.h"

#include <QHash>

static QHash<TokenType, OperatorInfo> OPINFO = {

    // Prefix operátorok
    { TokenType::Return,      { 6, false, Fixity::Prefix, 1 } },
    { TokenType::Function,    { 6, false, Fixity::Prefix, -1 } }, // -1 = variadic
    { TokenType::PrefixPlus,  { 6, false, Fixity::Prefix, 1 } },
    { TokenType::PrefixMinus, { 6, false, Fixity::Prefix, 1 } },

    // Multiplikatív
    { TokenType::Star,        { 5, false, Fixity::Infix, 2 } },
    { TokenType::Slash,       { 5, false, Fixity::Infix, 2 } },

    // Additív
    { TokenType::Plus,       { 4, false, Fixity::Infix, 2 } },
    { TokenType::Minus,      { 4, false, Fixity::Infix, 2 } },

    // Optional (??) – fontos: erősebb, mint +/-
    // Optional infix operátor: flag ?? expr
    { TokenType::OptionalQuestion, { 3, false, Fixity::Infix, 2 } },

    //Relációk
    { TokenType::Greater,        { 2, false, Fixity::Infix, 2 } },
    { TokenType::Less,           { 2, false, Fixity::Infix, 2 } },
    { TokenType::GreaterEqual,   { 2, false, Fixity::Infix, 2 } },
    { TokenType::LessEqual,      { 2, false, Fixity::Infix, 2 } },
    { TokenType::Equal,          { 2, false, Fixity::Infix, 2 } },

    //Assignment
    { TokenType::Assign,     { 1, true,  Fixity::Infix, 2 } }, // jobbról asszociatív

    // Ternary
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
