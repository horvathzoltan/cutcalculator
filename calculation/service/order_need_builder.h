#pragma once

#include <QVector>
#include <QUuid>

#include "need_calculator.h"
#include "orders/model/order.h"        // új domain Order
#include "orders/model/order_item.h"   // új domain OrderItem

/* ============================================================
 * 🧩 OrderNeedBuilder – Order → OrderNeed pipeline
 * ============================================================ */

class OrderNeedBuilder {
public:
    // domain Order-ből dolgozunk
    static OrderNeed build(const Order& order, bool debug = false);
};
