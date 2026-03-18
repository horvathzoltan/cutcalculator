#include "order_need_builder.h"

/* ============================================================
 * 🧩 OrderNeedBuilder::build
 * ============================================================ */

OrderNeed OrderNeedBuilder::build(const Order& order, bool debug)
{
    OrderNeed out;
    out.orderId = order.orderId;

    for (const auto& item : order.items) {

        OrderItemNeed oin;
        oin.orderItemId = item.orderItemId;
        oin.line = item.line;
        oin.itemNeeds.reserve(item.qty);

        for (int i = 0; i < item.qty; ++i) {
            ItemNeed n = NeedCalculator::calculate(item.line, item.modeName, debug);
            oin.itemNeeds.append(n);
        }

        out.orderItemNeeds.append(oin);
    }

    return out;
}
