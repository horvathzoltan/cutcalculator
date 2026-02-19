#pragma once
#include <QVector>
#include "ast.h"

class NodePool {
public:
    AstNode* create(AstNode::Type t, const QString& v) {
        AstNode* n = new AstNode(t, v);
        nodes.append(n);
        return n;
    }

    ~NodePool() {
        for (auto n : nodes)
            delete n;
    }

private:
    QVector<AstNode*> nodes;
};
