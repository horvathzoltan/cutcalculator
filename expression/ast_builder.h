#pragma once
#include <QVector>
#include "expression/node_pool.h"
#include "token.h"
#include "ast.h"

class AstBuilder {
public:
    static AstNode* fromRpn(const QVector<Token>& rpn, NodePool& pool);
};
