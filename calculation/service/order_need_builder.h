#pragma once

#include <QVector>
#include <QUuid>

#include "need_calculator.h"
#include "orders/model/order_header.h"
#include "orders/model/order_item.h"   // új domain OrderItem

/* ============================================================
 * 🧩 OrderNeedBuilder – Order → OrderNeed pipeline
 * ============================================================ */

class OrderNeedBuilder {
public:
    // kényelmes API – registry lookup
    static OrderNeed build(const QUuid& headerId, bool debug = false);

private:
    // tiszta, tesztelhető függvény
    static OrderNeed buildImpl(const OrderHeader& header,
                               const QVector<OrderItem>& items,
                               bool debug);

    // a teszt hozzáférhet a priváthoz
    friend class OrderNeedBuilderTest;
};
