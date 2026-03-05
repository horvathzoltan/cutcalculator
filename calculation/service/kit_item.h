#pragma once

#include <QString>
#include <QUuid>

struct KitItem {
    QUuid materialId;
    QString materialBarcode;
    int quantity;

    QString ownerName;
    QString colorName;

    int fullWidth;
    int fullHeight;
};
