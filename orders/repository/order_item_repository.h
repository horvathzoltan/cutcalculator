#pragma once

#include <QVector>
#include <optional>

#include "orders/model/order_item.h"
#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"

class OrderItemRepository {
public:
    static bool load(QVector<OrderItem>& out);
    static bool save(const QVector<OrderItem>& data);

private:
    struct Row {
        QString idStr;
        QString orderIdStr;
        QString productIdStr;
        QString width;
        QString height;
        QString handlerSide;
        QString externalId;
        QString ownerName;
        QString colorName;
        QString order_qty;
        QString modeName;
    };

    static std::optional<CsvImporter::AuditedRow<Row>>
    convertRowToRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);

    static QVector<CsvImporter::RowError>
    validateRow(const Row& row, int lineNumber);

    static std::optional<OrderItem>
    buildFromRow(const Row& row, CsvImporter::FileContext& ctx);

    static QVector<CsvImporter::AuditedRow<Row>>
    loadRows(CsvImporter::FileContext& ctx);

    static QString toCsvLine(const OrderItem& it);
};
