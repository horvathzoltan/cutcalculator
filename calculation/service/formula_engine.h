#pragma once
#include <QString>

struct EvaluatedFormula {
    int length_mm = 0;
    int pieces = 0;
    QString stringValue = "";
};

class FormulaEngine {
public:
    static EvaluatedFormula eval(const QString& f, int w, int h, int qty);

    static EvaluatedFormula evalChoose(const QString& t, int w, int h, int qty);
    static EvaluatedFormula evalWithOpt(const QString& t, int w, int h, int qty);

};
