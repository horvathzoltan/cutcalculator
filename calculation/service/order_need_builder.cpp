#include "order_need_builder.h"

//#include "orders/model/order_item.h"


#include "need_calculator.h"

#include <orders/registry/order_header_registry.h>
#include <orders/registry/order_item_registry.h>

/* ============================================================
 * 🧩 OrderNeedBuilder::build – domain Order → OrderNeed
 * ============================================================ */

OrderNeed OrderNeedBuilder::build(const QUuid& headerId, bool debug)
{
    auto* header = OrderHeaderRegistry::instance().findById(headerId);
    if (!header)
        return {};

    auto items = OrderItemRegistry::instance().findByOrderId(headerId);

    return buildImpl(*header, items, debug);
}

OrderNeed OrderNeedBuilder::buildImpl(const OrderHeader& header,
                                      const QVector<OrderItem>& items,
                                      bool debug)
{
    OrderNeed out;
    out.orderId = header.id;

    for (const auto& item : items) {
        OrderItemNeed oin;
        oin.orderItemId = item.id;

        oin.itemNeeds.reserve(item.order_qty);
        for (int i = 0; i < item.order_qty; ++i) {
            ItemNeed n = NeedCalculator::calculate(item, item.modeName, debug);
            oin.itemNeeds.append(n);
        }

        out.orderItemNeeds.append(oin);
    }

    return out;
}

