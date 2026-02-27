#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

#include "piece.h"

struct CutAggregatedItem
{
    QString materialBarcode;
    int requiredLength;
    QString handlerSide;
    int quantity;

    QStringList externalRefs;
    QString ownerName;

    int fullWidth;
    int fullHeight;
};

class CutAggregator
{
public:
    static QVector<CutAggregatedItem> aggregate(const QVector<Piece>& pieces);
};
