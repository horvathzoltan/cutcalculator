#pragma once
#include <QUuid>
#include <QVector>
#include "calculation/service/matrix_missing_detail.h"

class MatrixGenerator {
public:
    // v2: generate missing details in deterministic order (product → mode → material)
    static bool generate(const QVector<MissingDetail>& list);

    // v2: generate all missing details for a product (deterministic)
    static bool generateForProduct(const QUuid& productId);

    // v2: generate all missing details for a mode (deterministic)
    static bool generateForMode(const QUuid& modeId);
};
