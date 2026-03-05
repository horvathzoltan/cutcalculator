#include "kit_aggregator.h"
#include <QMap>

QVector<KitAggregatedItem> KitAggregator::aggregate(const QVector<KitItem>& items)
{
    QMap<QString, KitAggregatedItem> map;

    for (const KitItem& it : items) {

        QString barcode = it.materialBarcode;

        if (!map.contains(barcode)) {

            KitAggregatedItem agg;
            agg.materialBarcode = it.materialBarcode;
            agg.quantity = it.quantity;

            agg.ownerName  = it.ownerName;
            agg.colorName  = it.colorName;
            agg.fullWidth  = it.fullWidth;
            agg.fullHeight = it.fullHeight;

            map.insert(barcode, agg);

        } else {
            auto& agg = map[barcode];
            agg.quantity += it.quantity;
        }
    }

    return map.values().toVector();
};
