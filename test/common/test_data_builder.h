#pragma once
#include "calcmodes/model/need_calculation.h"
#include "calculation/model/need_calculation_detail.h"
#include <QUuid>

struct TestDataIds {
    QUuid P1;
    QUuid M1;
    QUuid M2;

    QString P1_barcode;
    QString M1_barcode;
    QString M2_barcode;
};

class TestDataBuilder {
public:
    static TestDataIds prepareStandard();

    static NeedCalculationDetail makeDetail(
        QUuid calcId,
        QUuid materialId,
        const QString &formula,
        NeedCalculationDetail::DetailKind kind);

    static NeedCalculation makeCalculation(
        QUuid productId,
        const QString& name);
};
