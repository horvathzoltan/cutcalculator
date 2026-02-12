#pragma once
#include <QUuid>

struct MissingDetail {
    QUuid productId;
    QString productName;
    QUuid modeId;
    QString modeName;
    QUuid materialId;
    QString materialName;
    QString materialBarcode;
};

