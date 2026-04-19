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
        const OrderLine& line = oin.line;

        for (const auto& itemNeed : oin.itemNeeds) {

            // --- Cutting: RawCut → Piece ---
            for (const RawCut& rc : itemNeed.cutItems) {
                Piece p;
                p.materialBarcode = rc.materialBarcode;
                p.requiredLength  = rc.requiredLength;
                p.handlerSide     = line.handlerSide;
                p.externalRef     = line.externalId;
                p.ownerName       = line.ownerName;
                p.colorName       = line.colorName;
                p.fullWidth       = line.width_mm;
                p.fullHeight      = line.height_mm;
                pieces.append(p);
            }

            // --- Kitting: RawKit → KitItem ---
            for (const RawKit& rk : itemNeed.kitItems) {
                KitItem ki;
                ki.materialBarcode = rk.materialBarcode;
                ki.quantity        = rk.kitting_qty;
                ki.ownerName       = line.ownerName;
                ki.colorName       = line.colorName;
                ki.fullWidth       = line.width_mm;
                ki.fullHeight      = line.height_mm;
                kits.append(ki);
            }
        }
    }

    ws.cuts = CutAggregator::aggregate(pieces);
    ws.kits = KitAggregator::aggregate(kits);

    return ws;
}
