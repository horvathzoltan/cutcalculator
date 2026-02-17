#pragma once
#include <QVector>
#include "token.h"
#include "ast.h"

class AstBuilder {
public:
    static AstNode* fromRpn(const QVector<Token>& rpn);
};
