#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

#include "need_calculator.h"

struct KitAggregatedItem {
    QUuid materialId;
    QString materialBarcode;
    int quantity;

    QString ownerName;
    QString colorName;

    int fullWidth;
    int fullHeight;
};

class KitAggregator
{
public:
    static QVector<KitAggregatedItem> aggregate(const QVector<KitItem>& items);
};



