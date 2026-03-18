#pragma once

#include <QVector>
#include <QUuid>
#include "need_calculator.h"

/* ============================================================
 * 🧩 OrderNeedBuilder – Order → OrderNeed pipeline
 * ============================================================ */

struct OrderItem {
    QUuid orderItemId;
    OrderLine line;
    int qty = 1;
    QString modeName;
};

struct Order {
    QUuid orderId;
    QVector<OrderItem> items;
};

class OrderNeedBuilder {
public:
    static OrderNeed build(const Order& order, bool debug = false);
};
