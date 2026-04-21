#include "order_need_builder.h"

#include "orders/model/order.h"
#include "orders/model/order_item.h"
#include "orders/model/order_line.h"

#include "need_calculator.h"

/* ============================================================
 * 🧩 OrderNeedBuilder::build – domain Order → OrderNeed
 * ============================================================ */

OrderNeed OrderNeedBuilder::build(const Order& order, bool debug)
{
    OrderNeed out;
    out.orderId = order.id;

    // Végigmegyünk a domain OrderItem-eken
    for (const auto& item : order.items) {

        OrderItemNeed oin;
        oin.orderItemId = item.id;

        // --- Domain OrderLine → CalcOrderLine leképezés ---
        CalcOrderLine line;
        line.productId   = item.line.productId;
        line.width_mm    = item.line.width_mm;
        line.height_mm   = item.line.height_mm;

        // Domain mezők → technikai mezők
        line.handlerSide = item.line.overrideHandlerSide.isEmpty()
                               ? order.header.defaultHandlerSide
                               : item.line.overrideHandlerSide;

        line.externalId  = item.line.overrideExternalId;
        line.ownerName   = order.header.customerName;

        line.colorName   = item.line.overrideColor.isEmpty()
                             ? order.header.defaultColor
                             : item.line.overrideColor;

        oin.line = line;

        // --- qty példányosítás ---
        oin.itemNeeds.reserve(item.qty);
        for (int i = 0; i < item.qty; ++i) {
            ItemNeed n = NeedCalculator::calculate(line, item.modeName, debug);
            oin.itemNeeds.append(n);
        }

        out.orderItemNeeds.append(oin);
    }

    return out;
}
