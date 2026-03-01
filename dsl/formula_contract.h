#pragma once

#include <QString>
#include <QSet>

struct FormulaContract {
    QSet<QString> allowedInputs;
    QSet<QString> requiredOutputs;
    QSet<QString> optionalOutputs;
};

// Cutting szerződés
FormulaContract cuttingContract();

// Kitting szerződés
FormulaContract kittingContract();
