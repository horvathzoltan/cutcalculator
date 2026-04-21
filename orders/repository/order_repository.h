#pragma once

#include <QString>
#include <QVector>
#include <optional>

#include "orders/model/order.h"

/**
 * OrderRepository – CSV alapú perzisztencia.
 *
 * Felelősségek:
 *   - Order → CSV sorok (save)
 *   - CSV sorok → Order (load)
 *   - háromfázisú import pipeline:
 *        1) Convert   (CSV → OrderRow)
 *        2) Validate  (mezők ellenőrzése)
 *        3) Build     (OrderRow → OrderHeader + OrderItem)
 *        4) Assemble  (több sor → Order)
 */
class OrderRepository {
public:
    std::optional<Order> load(const QString& filePath);
    bool save(const Order& order, const QString& filePath);

private:
    // 1) CSV sor → OrderRow (nyers adat)
    struct OrderRow {
        QString orderId;
        QString customerName;
        QString orderDate;
        QString deadline;
        QString defaultColor;
        QString defaultHandlerSide;
        QString defaultExternalPrefix;
        QString note;
        QString status;

        QString itemId;
        QString productId;
        QString width_mm;
        QString height_mm;
        QString handlerSide;
        QString externalId;
        QString ownerName;
        QString colorName;
        QString qty;
        QString modeName;
    };

    std::optional<OrderRow> convertRow(const QStringList& cols) const;
    bool validateRow(const OrderRow& row, QString& error) const;
    Order buildOrder(const QVector<OrderRow>& rows) const;

    QVector<OrderRow> assembleRows(const Order& order) const;
};
