#include "orders/repository/order_item_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QFile>
#include <QTextStream>
#include <QUuid>

// ------------------------------------------------------------
// Convert Phase
// ------------------------------------------------------------
std::optional<CsvImporter::AuditedRow<OrderItemRepository::Row>>
OrderItemRepository::convertRowToRow(
    const QVector<QString>& parts,
    CsvImporter::FileContext& ctx)
{
    if (parts.size() < 11) {
        ctx.addError(ctx.currentLineNumber(),
                     "⚠️ Kevés mező (11 szükséges)");
        return std::nullopt;
    }

    Row r {
        parts[0].trimmed(),
        parts[1].trimmed(),
        parts[2].trimmed(),
        parts[3].trimmed(),
        parts[4].trimmed(),
        parts[5].trimmed(),
        parts[6].trimmed(),
        parts[7].trimmed(),
        parts[8].trimmed(),
        parts[9].trimmed(),
        parts[10].trimmed()
    };

    return CsvImporter::AuditedRow<Row>{ ctx.currentLineNumber(), r };
}

// ------------------------------------------------------------
// Validate Phase
// ------------------------------------------------------------
QVector<CsvImporter::RowError>
OrderItemRepository::validateRow(const Row& row, int lineNumber)
{
    QVector<CsvImporter::RowError> errors;

    if (row.orderIdStr.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó orderId" });

    if (row.productIdStr.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó productId" });

    if (row.externalId.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó externalId" });

    return errors;
}

// ------------------------------------------------------------
// Build Phase
// ------------------------------------------------------------
std::optional<OrderItem>
OrderItemRepository::buildFromRow(
    const Row& row,
    CsvImporter::FileContext& ctx)
{
    auto errs = validateRow(row, ctx.currentLineNumber());
    ctx.addErrors(errs);
    if (!errs.isEmpty())
        return std::nullopt;

    OrderItem it;

    it.id = QUuid(row.idStr);
    if (it.id.isNull())
        it.id = QUuid::createUuid();

    it.orderId = QUuid(row.orderIdStr);
    it.productId = QUuid(row.productIdStr);

    it.width_mm = row.width.toInt();
    it.height_mm = row.height.toInt();

    it.handlerSide = row.handlerSide;
    it.externalId = row.externalId;
    it.ownerName = row.ownerName;
    it.colorName = row.colorName;

    it.order_qty = row.order_qty.toInt();
    if (it.order_qty <= 0)
        it.order_qty = 1;

    it.modeName = row.modeName;

    // name = externalId (IdentifiableEntity)
    it.name = it.externalId;

    return it;
}

// ------------------------------------------------------------
// Assemble Phase
// ------------------------------------------------------------
QVector<CsvImporter::AuditedRow<OrderItemRepository::Row>>
OrderItemRepository::loadRows(CsvImporter::FileContext& ctx)
{
    return CsvImporter::readAndConvert<
        CsvImporter::AuditedRow<Row>
        >(ctx, convertRowToRow);
}

// ------------------------------------------------------------
// LOAD
// ------------------------------------------------------------
bool OrderItemRepository::load(QVector<OrderItem>& out)
{
    auto path = FileNameHelper::instance().pathFor(FileKind::OrderItems, FileAccess::Read);
    CsvImporter::FileContext ctx("OrderItem import", path);

    const auto rows = loadRows(ctx);

    QVector<OrderItem> defs =
        CsvImporter::buildAll<Row, OrderItem>(
            rows,
            buildFromRow,
            ctx
            );

    out = defs;

    zInfo(QString("📦 OrderItemRepository: %1 rekord betöltve").arg(defs.size()));
    return true;
}

// ------------------------------------------------------------
// SAVE
// ------------------------------------------------------------
bool OrderItemRepository::save(const QVector<OrderItem>& data)
{
    auto path = FileNameHelper::instance().pathFor(FileKind::OrderItems, FileAccess::Write);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);

    out << "id;orderId;productId;width;height;handlerSide;"
           "externalId;ownerName;colorName;order_qty;modeName\n";

    for (const auto& it : data)
        out << toCsvLine(it) << "\n";

    return true;
}

QString OrderItemRepository::toCsvLine(const OrderItem& it)
{
    return QString("%1;%2;%3;%4;%5;%6;%7;%8;%9;%10;%11")
    .arg(it.id.toString(QUuid::WithoutBraces))
        .arg(it.orderId.toString(QUuid::WithoutBraces))
        .arg(it.productId.toString(QUuid::WithoutBraces))
        .arg(it.width_mm)
        .arg(it.height_mm)
        .arg(it.handlerSide)
        .arg(it.externalId)
        .arg(it.ownerName)
        .arg(it.colorName)
        .arg(it.order_qty)
        .arg(it.modeName);
}
