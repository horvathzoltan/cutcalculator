#pragma once

#include <QString>
#include <QSet>

struct FormulaContract {
    QSet<QString> allowedInputs;
    QSet<QString> requiredOutputs;
    QSet<QString> optionalOutputs;
};

// Cutting szerződés
// material output: STRING típus (barcode)
FormulaContract cuttingContract();

// Kitting szerződés
// material output: STRING típus (barcode)
FormulaContract kittingContract();

