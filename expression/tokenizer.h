#pragma once
#include <QString>
#include <QVector>
#include "token.h"

class Tokenizer {
public:
    static QVector<Token> tokenize(const QString& input);
};
