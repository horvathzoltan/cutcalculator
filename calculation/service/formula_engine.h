#pragma once
#include <QString>

struct EvaluatedFormula {
    int length_mm = 0;
    int pieces = 0;
};

class FormulaEngine {
public:
    static EvaluatedFormula eval(const QString& f, int w, int h, int qty);
};
