#include "order_repository.h"
#include <QFile>
#include <QTextStream>

std::optional<OrderRepository::OrderRow>
OrderRepository::convertRow(const QStringList& cols) const
{
    if (cols.size() < 20)
        return std::nullopt;

    OrderRow r;
    int i = 0;

    r.orderId               = cols[i++];
    r.customerName          = cols[i++];
    r.orderDate             = cols[i++];
    r.deadline              = cols[i++];
    r.defaultColor          = cols[i++];
    r.defaultHandlerSide    = cols[i++];
    r.defaultExternalPrefix = cols[i++];
    r.note                  = cols[i++];
    r.status                = cols[i++];

    r.itemId                = cols[i++];
    r.productId             = cols[i++];
    r.width_mm              = cols[i++];
    r.height_mm             = cols[i++];
    r.handlerSide           = cols[i++];
    r.externalId            = cols[i++];
    r.ownerName             = cols[i++];
    r.colorName             = cols[i++];
    r.qty                   = cols[i++];
    r.modeName              = cols[i++];

    return r;
}
