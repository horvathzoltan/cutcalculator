#include "worksheet_builder.h"
#include "piece.h"
#include "kit_item.h"

/* ============================================================
 * 🧩 WorksheetBuilder::fromOrderNeed
 * ============================================================ */

WorksheetNeed WorksheetBuilder::fromOrderNeed(const OrderNeed& orderNeed)
{
    WorksheetNeed ws;
    ws.worksheetId = orderNeed.orderId;

    QVector<Piece> pieces;
    QVector<KitItem> kits;

    for (const auto& oin : orderNeed.orderItemNeeds) {

        const OrderItem& item = oin.item;

        for (const auto& itemNeed : oin.itemNeeds) {

            // --- Cutting: RawCut → Piece ---
            for (const RawCut& rc : itemNeed.cutItems) {
                Piece p;
                p.materialBarcode = rc.materialBarcode;
                p.requiredLength  = rc.requiredLength;

                // OrderItem metaadatok
                p.handlerSide     = item.handlerSide;
                p.externalRef     = item.externalId;
                p.ownerName       = item.ownerName;
                p.colorName       = item.colorName;
                p.fullWidth       = item.width_mm;
                p.fullHeight      = item.height_mm;

                pieces.append(p);
            }

            // --- Kitting: RawKit → KitItem ---
            for (const RawKit& rk : itemNeed.kitItems) {
                KitItem ki;
                ki.materialBarcode = rk.materialBarcode;
                ki.quantity        = rk.kitting_qty;

                // OrderItem metaadatok
                ki.ownerName       = item.ownerName;
                ki.colorName       = item.colorName;
                ki.fullWidth       = item.width_mm;
                ki.fullHeight      = item.height_mm;

                kits.append(ki);
            }
        }
    }

    ws.cuts = CutAggregator::aggregate(pieces);
    ws.kits = KitAggregator::aggregate(kits);

    return ws;
}

