#pragma once
#include <QUuid>
#include "calculation/service/matrix_missing_detail.h"


class MatrixValidator {
public:
    static QVector<MissingDetail> validateProduct(const QUuid& productId);
    static QVector<MissingDetail> validateMode(const QUuid& modeId);
    static QVector<MissingDetail> validateAll();


    // v2: matrix complete if every NeedRule has a detail AND the formula is valid
    static bool isCalculationMatrixComplete(const QUuid& calcId);

    // v2: product matrix complete if no mode has missing details
    static bool isProductMatrixComplete(const QUuid& productId);
};
