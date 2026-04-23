#include "orders/registry/order_header_registry.h"
#include "orders/repository/order_header_repository.h"

// ------------------------------------------------------------
// Domain validation
// ------------------------------------------------------------
bool OrderHeaderRegistry::validateDomain(const OrderHeader& h) const
{
    if (h.customerName.isEmpty())
        return false;

    return true;
}

bool OrderHeaderRegistry::validateDuplicate(const OrderHeader& h) const
{
    return !existsBy([&](const OrderHeader& x){
        return x.customerName == h.customerName && x.id != h.id;
    });
}

// ------------------------------------------------------------
// Workflow hooks
// ------------------------------------------------------------
bool OrderHeaderRegistry::beforeInsert(OrderHeader& h)
{
    if (h.id.isNull())
        h.id = QUuid::createUuid();

    if (h.name.isEmpty())
        h.name = h.customerName;

    return true;
}

bool OrderHeaderRegistry::beforeUpdate(OrderHeader& h)
{
    if (h.name.isEmpty())
        h.name = h.customerName;

    return true;
}

// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------
void OrderHeaderRegistry::onInsertLog(const OrderHeader& h)
{
    zInfo(QString("➕ OrderHeader INSERT: %1 (%2)")
              .arg(h.id.toString(), h.customerName));
}

void OrderHeaderRegistry::onUpdateLog(const OrderHeader& h)
{
    zInfo(QString("✏️ OrderHeader UPDATE: %1 (%2)")
              .arg(h.id.toString(), h.customerName));
}

void OrderHeaderRegistry::onRemoveLog(const OrderHeader& h)
{
    zInfo(QString("🗑️ OrderHeader REMOVE: %1 (%2)")
              .arg(h.id.toString(), h.customerName));
}

// ------------------------------------------------------------
// Persist
// ------------------------------------------------------------
void OrderHeaderRegistry::persist() const
{
    OrderHeaderRepository::save(readAll());
}
