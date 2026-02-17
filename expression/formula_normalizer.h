#pragma once
#include <QString>

class FormulaNormalizer {
public:
    static QString stripOuterParens(const QString& s);
    static QString normalizeWhitespace(const QString& s);
    static bool isNumberLiteral(const QString& s);
    static QString normalizeCondition(const QString& cond);
};
