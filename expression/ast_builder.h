#pragma once
#include <QVector>
#include "common/utils/result.h"
#include "expression/node_pool.h"
#include "token.h"
#include "ast.h"

class AstBuilder {
public:
    static Result<AstNode*> fromRpn(const QVector<Token>& rpn, NodePool& pool);
};
