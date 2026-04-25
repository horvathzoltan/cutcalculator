#include "order_item_table.h"

OrderItemTable::OrderItemTable(QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(8);
    setupColumns();
}

void OrderItemTable::setupColumns()
{
    QStringList headers = {
        "Product",
        "Width (mm)",
        "Height (mm)",
        "Handler side",
        "Color",
        "External ID",
        "Qty",
        "Mode"
    };

    setHorizontalHeaderLabels(headers);
}
