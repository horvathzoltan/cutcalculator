#pragma once

#include <QString>

struct CutKey
{
    QString materialBarcode;
    int requiredLength;
    QString handlerSide;

    bool operator<(const CutKey& other) const;
    bool operator==(const CutKey& other) const;
};
