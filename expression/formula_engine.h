#pragma once
#include <QString>
#include "expression/eval_result.h"
#include "value.h"
#include "ast.h"

class FormulaEngine {
public:
    static EvalResult eval(const QString& code);

private:
    static Value evalNode(AstNode* n, EvalResult* traceOut);
    static QVector<Value> evalChildren(AstNode* n, EvalResult* traceOut);
    static EvalResult evalSingleLine(const QString &code);
    static EvalResult evalMultiLine(const QStringList &lines);
};

