#pragma once

#include <QVector>
#include "token.h"
#include "common/utils/result.h"

class Parser {
public:

    struct ParseResult {
        QVector<Token> tokens;
        QVector<Token> rpn;
    };


    // convenience: string -> tokens -> RPN
    static Result<ParseResult> parse(const QString& input);

    // ha már van tokenized input
    static Result<QVector<Token>> toRpn(const QVector<Token>& tokens);
    static void debugDump(const ParseResult &pr, const QString &code, const QString &error);
};
