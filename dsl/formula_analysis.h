#pragma once

#include <QString>
#include <QStringList>
#include <QSet>

#include "formula_contract.h"

// Elemzés eredménye
struct FormulaAnalysis {
    bool ok = false;
    QStringList errors;
    QStringList warnings;

    QSet<QString> readVars;
    QSet<QString> writtenVars;
    QSet<QString> calledFunctions;
};

// Fő elemző függvény
FormulaAnalysis analyzeFormula(const QString& formula,
                               const FormulaContract& contract);
