#pragma once

#include <QVector>
#include "token.h"

class Parser {
public:

    struct ParseResult {
        QVector<Token> tokens;
        QVector<Token> rpn;
    };


    // convenience: string -> tokens -> RPN
    static ParseResult parse(const QString& input);

    // ha már van tokenized input
    static QVector<Token> toRpn(const QVector<Token>& tokens);
};
