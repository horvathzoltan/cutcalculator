#pragma once
#include <QObject>
#include <QUuid>

#include "orders/model/order_header.h"
#include "orders/model/order_item.h"
#include "orders/registry/order_header_registry.h"
#include "orders/registry/order_item_registry.h"

class OrderManager : public QObject {
    Q_OBJECT
public:
    explicit OrderManager(QObject* parent = nullptr);

    std::optional<OrderHeader> loadHeader(const QUuid& id) const;
    QVector<OrderItem> loadItems(const QUuid& id) const;

    bool saveOrder(const OrderHeader& header,
                   const QVector<OrderItem>& items);

    bool deleteOrder(const QUuid& id);

    bool createItem(const OrderItem& it);
    bool deleteItem(const QUuid& itemId);

signals:
    void orderSaved(const QUuid& id);
    void orderDeleted(const QUuid& id);
    void validationFailed(const QString& message);
};
