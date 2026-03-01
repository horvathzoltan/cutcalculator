#pragma once
#include <QString>
#include <QVector>
#include <common/utils/result.h>
#include "token.h"

class Tokenizer {
public:
    static Result<QVector<Token>> tokenize(const QString& input);
};
