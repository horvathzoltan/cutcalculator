#pragma once

#include <QVector>
#include "token.h"

class Parser {
public:
    // convenience: string -> tokens -> RPN
    static QVector<Token> parseToRpn(const QString& input);

    // ha már van tokenized input
    static QVector<Token> toRpn(const QVector<Token>& tokens);

private:
    static int precedence(const Token& t);
    static bool isLeftAssociative(TokenType t);
    static bool isOperator(TokenType t);
};
