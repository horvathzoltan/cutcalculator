#include "cut_aggregator.h"
#include <QMap>

QVector<CutAggregatedItem>
CutAggregator::aggregate(const QVector<Piece>& pieces)
{
    QMap<QString, CutAggregatedItem> map;

    for (const Piece& p : pieces) {

        // Kulcs: barcode + length + handlerSide
        QString key = QString("%1|%2|%3")
                          .arg(p.materialBarcode)
                          .arg(p.requiredLength)
                          .arg(p.handlerSide);

        if (!map.contains(key)) {
            CutAggregatedItem item;
            item.materialBarcode = p.materialBarcode;
            item.requiredLength  = p.requiredLength;
            item.handlerSide     = p.handlerSide;
            item.quantity        = 1;

            item.externalRefs = QStringList{ p.externalRef };
            item.ownerName    = p.ownerName;
            item.fullWidth    = p.fullWidth;
            item.fullHeight   = p.fullHeight;

            map.insert(key, item);
        } else {
            auto& item = map[key];
            item.quantity += 1;
            item.externalRefs.append(p.externalRef);
        }
    }

    return map.values().toVector();
}
