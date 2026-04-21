#pragma once

#include <QUuid>
#include <QVector>

#include "orders/model/order_header.h"
#include "orders/model/order_item.h"

/**
 * Order – egy teljes megrendelés domain modellje.
 *
 * A rendelés két részből áll:
 *   1) OrderHeader – a rendelés szintű metaadatok (megrendelő, határidő, default értékek)
 *   2) OrderItem-ek – a tényleges megrendelt tételek
 *
 * A rendszerben ez az entitás a "megrendelés" fogalmának egyetlen igazságforrása.
 * Minden további pipeline (Need, Worksheet, WorkOrder) ebből indul ki.
 */
struct Order {
    QUuid id;
    // A rendelés egyedi technikai azonosítója.
    // A teljes pipeline erre hivatkozik (OrderNeed.orderId, WorksheetNeed.worksheetId, stb.).

    OrderHeader header;
    // A rendelés fejadatai (megrendelő neve, határidő, default szín, default handler, stb.).
    // Minden OrderItem örökli a releváns mezőket (ownerName, defaultColor, stb.).

    QVector<OrderItem> items;
    // A rendelés tételei.
    // Minden OrderItem egy teljesen kitöltött, véglegesített megrendelési sor.
    // A NeedCalculator közvetlenül ezeket dolgozza fel.

    bool isEmpty() const {
        return items.isEmpty();
    }
    // Gyors ellenőrzés: van-e egyáltalán tétel a rendelésben.
};
