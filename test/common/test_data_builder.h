#pragma once
#include "calcmodes/model/need_calculation.h"
#include "calculation/model/need_calculation_detail.h"
#include <QUuid>

struct TestDataIds {
    QUuid P1;
    QUuid M1;
    QUuid M2;
};

class TestDataBuilder {
public:
    static TestDataIds prepareStandard();

    static NeedCalculationDetail makeDetail(
        QUuid calcId,
        QUuid materialId,
        const QString &formula);

    static NeedCalculation makeCalculation(
        QUuid productId,
        const QString& name);
};
