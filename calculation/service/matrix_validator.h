#pragma once
#include <QUuid>

class MatrixValidator {
public:
    static bool isCalculationMatrixComplete(const QUuid& calcId);
    static bool isProductMatrixComplete(const QUuid& productId);
};
