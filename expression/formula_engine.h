#pragma once
#include <QString>

struct EvaluatedFormula {
    int length_mm = 0;
    int pieces = 0;
    QString stringValue = "";
};

class FormulaEngine {
public:
    static EvaluatedFormula eval(const QString& f);

    static EvaluatedFormula evalChoose(const QString& t);
    static EvaluatedFormula evalWithOpt(const QString& t);

};
