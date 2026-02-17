#include "formula_normalizer.h"

QString FormulaNormalizer::stripOuterParens(const QString& s)
{
    QString t = s.trimmed();
    if (t.startsWith('(') && t.endsWith(')'))
        return t.mid(1, t.size() - 2).trimmed();
    return t;
}

QString FormulaNormalizer::normalizeWhitespace(const QString& s)
{
    QString t = s;
    t.replace("\t", " ");
    while (t.contains("  "))
        t.replace("  ", " ");
    return t.trimmed();
}

bool FormulaNormalizer::isNumberLiteral(const QString& s)
{
    bool ok = false;
    s.toDouble(&ok);
    return ok;
}

QString FormulaNormalizer::normalizeCondition(const QString& cond)
{
    QString t = normalizeWhitespace(cond);
    t = stripOuterParens(t);
    t.remove(' ');
    return t;
}
