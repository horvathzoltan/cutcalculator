#pragma once

#include <QString>
#include <QVariant>
#include "keyword.h"

class Expression{
public:
    Expression();
    Expression(KeyWord::Word keyword, const QList<QVariant>& params,const QString& original_text);

    static Expression Parse(const QString& txt);
    QString ToString() const;
    bool isValid(){return _isValid;}
    QVariant Calculate();
private:
    bool _isValid;
    KeyWord::Word _keyword;
    QList<QVariant> _params;
    QString _original_text;
};

