#pragma once
#include <QString>
#include <QVariant>
#include <QVector>
#include "token.h"

struct AstNode {
    enum class Type {
        Number,
        Variable,
        Function,
        Operator
    };

    Type type;
    QString value;               // pl. "mul", "+", "w", "10"
    QVector<AstNode*> children;  // bináris operátor: 2 elem, függvény: N elem

    AstNode(Type t, const QString& v)
        : type(t), value(v) {}
};
