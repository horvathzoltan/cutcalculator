#pragma once
#include "expression/token.h"

enum class Fixity {
    Prefix,
    Infix,
    Postfix,
    Ternary
};

struct OperatorInfo {
    int precedence = -1;
    bool rightAssociative = false;
    Fixity fixity = Fixity::Infix;
    int operandCount = 2;
};

bool isOperator(const Token& t);
const OperatorInfo* getOpInfo(const Token& t);
