#pragma once

#include <QVector>
#include <optional>

#include "orders/model/order_header.h"
#include "common/csv/csvimporter.h"
#include "common/csv/filecontext.h"

class OrderHeaderRepository {
public:
    static bool load(QVector<OrderHeader>& out);
    static bool save(const QVector<OrderHeader>& data);

private:
    struct Row {
        QString idStr;
        QString customerName;
        QString orderDate;
        QString defaultColor;
        QString defaultHandlerSide;
        QString defaultExternalPrefix;
        QString note;
        QString status;
        QString deadline;
    };

    static std::optional<CsvImporter::AuditedRow<Row>>
    convertRowToRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);

    static QVector<CsvImporter::RowError>
    validateRow(const Row& row, int lineNumber);

    static std::optional<OrderHeader>
    buildFromRow(const Row& row, CsvImporter::FileContext& ctx);

    static QVector<CsvImporter::AuditedRow<Row>>
    loadRows(CsvImporter::FileContext& ctx);

    static QString toCsvLine(const OrderHeader& h);
};
