#include "orders/registry/order_item_registry.h"
#include "orders/repository/order_item_repository.h"
#include "orders/registry/order_header_registry.h"

// ------------------------------------------------------------
// Domain validation
// ------------------------------------------------------------
bool OrderItemRegistry::validateDomain(const OrderItem& it) const
{
    if (it.orderId.isNull())
        return false;

    if (it.productId.isNull())
        return false;

    if (it.order_qty <= 0)
        return false;

    return true;
}

bool OrderItemRegistry::validateDuplicate(const OrderItem& it) const
{
    return !existsBy([&](const OrderItem& x){
        return x.orderId == it.orderId &&
               x.externalId == it.externalId &&
               x.id != it.id;
    });
}

// ------------------------------------------------------------
// Workflow hooks
// ------------------------------------------------------------
bool OrderItemRegistry::beforeInsert(OrderItem& it)
{
    if (it.id.isNull())
        it.id = QUuid::createUuid();

    // Default öröklés a headerből
    auto* h = OrderHeaderRegistry::instance().findById(it.orderId);
    if (h) {
        if (it.ownerName.isEmpty())
            it.ownerName = h->customerName;

        if (it.colorName.isEmpty())
            it.colorName = h->defaultColor;

        if (it.handlerSide.isEmpty())
            it.handlerSide = h->defaultHandlerSide;

        // ExternalId generálás
        if (it.externalId.isEmpty()) {
            int maxId = 0;
            auto siblings = findAll([&](const OrderItem& x){
                return x.orderId == it.orderId;
            });

            for (const auto& s : siblings) {
                bool ok = false;
                int n = s.externalId.toInt(&ok);
                if (ok && n > maxId)
                    maxId = n;
            }

            it.externalId = QString::number(maxId + 1);
        }
    }

    // name = externalId
    if (it.name.isEmpty())
        it.name = it.externalId;

    return true;
}

bool OrderItemRegistry::beforeUpdate(OrderItem& it)
{
    if (it.name.isEmpty())
        it.name = it.externalId;

    return true;
}

// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------
void OrderItemRegistry::onInsertLog(const OrderItem& it)
{
    zInfo(QString("➕ OrderItem INSERT: %1 (%2)")
              .arg(it.id.toString(), it.externalId));
}

void OrderItemRegistry::onUpdateLog(const OrderItem& it)
{
    zInfo(QString("✏️ OrderItem UPDATE: %1 (%2)")
              .arg(it.id.toString(), it.externalId));
}

void OrderItemRegistry::onRemoveLog(const OrderItem& it)
{
    zInfo(QString("🗑️ OrderItem REMOVE: %1 (%2)")
              .arg(it.id.toString(), it.externalId));
}

// ------------------------------------------------------------
// Persist
// ------------------------------------------------------------
void OrderItemRegistry::persist() const
{
    OrderItemRepository::save(readAll());
}
