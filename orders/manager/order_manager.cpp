#include "orders/manager/order_manager.h"
#include "common/logger/event_logger.h"

OrderManager::OrderManager(QObject* parent)
    : QObject(parent)
{
}

std::optional<OrderHeader> OrderManager::loadHeader(const QUuid& id) const
{
    const auto* h = OrderHeaderRegistry::instance().findById(id);
    if (!h)
        return std::nullopt;
    return *h;
}

QVector<OrderItem> OrderManager::loadItems(const QUuid& id) const
{
    return OrderItemRegistry::instance().findByOrderId(id);
}

bool OrderManager::saveOrder(const OrderHeader& header,
                             const QVector<OrderItem>& items)
{
    // -------------------------
    // Header validáció
    // -------------------------
    if (header.customerName.trimmed().isEmpty()) {
        emit validationFailed("Customer name cannot be empty.");
        return false;
    }

    if (header.deadline < header.orderDate) {
        emit validationFailed("Deadline cannot be earlier than order date.");
        return false;
    }

    // -------------------------
    // Items validáció
    // -------------------------
    for (const auto& it : items) {

        if (it.productId.isNull()) {
            emit validationFailed("Each item must have a product selected.");
            return false;
        }

        if (it.width_mm <= 0 || it.height_mm <= 0) {
            emit validationFailed("Width and height must be positive.");
            return false;
        }

        if (it.order_qty <= 0) {
            emit validationFailed("Quantity must be at least 1.");
            return false;
        }

        if (it.modeName.trimmed().isEmpty()) {
            emit validationFailed("Each item must have a calculation mode.");
            return false;
        }

        // HandlerSide: "xJ-yB" → x+y == qty
        if (!it.handlerSide.isEmpty()) {
            auto parts = it.handlerSide.split("-");
            if (parts.size() == 2) {
                int j = parts[0].remove("J").toInt();
                int b = parts[1].remove("B").toInt();
                if (j + b != it.order_qty) {
                    emit validationFailed("Handler side J+B must equal quantity.");
                    return false;
                }
            }
        }
    }

    // -------------------------
    // Ha minden valid → mentés
    // -------------------------
    auto& headerReg = OrderHeaderRegistry::instance();
    auto& itemReg   = OrderItemRegistry::instance();

    if (!headerReg.existsById(header.id))
        headerReg.insert(header);
    else
        headerReg.update(header);

    // Remove old items
    auto oldItems = itemReg.findByOrderId(header.id);
    for (const auto& it : oldItems)
        itemReg.remove(it.id);

    // Insert new items
    for (auto it : items)
        itemReg.insert(it);

    emit orderSaved(header.id);
    return true;
}


bool OrderManager::deleteOrder(const QUuid& id)
{
    auto& headerReg = OrderHeaderRegistry::instance();
    auto& itemReg   = OrderItemRegistry::instance();

    // Remove items
    auto items = itemReg.findByOrderId(id);
    for (const auto& it : items)
        itemReg.remove(it.id);

    // Remove header
    headerReg.remove(id);

    emit orderDeleted(id);
    return true;
}
