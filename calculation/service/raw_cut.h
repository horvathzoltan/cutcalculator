#pragma once

#include <QString>


struct RawCut {
    QString materialBarcode;
    int requiredLength;
    int cutting_qty = 1;
};
