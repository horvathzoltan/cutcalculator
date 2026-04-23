#include "orders/repository/order_header_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QFile>
#include <QTextStream>
#include <QUuid>

// ------------------------------------------------------------
// Convert Phase
// ------------------------------------------------------------
std::optional<CsvImporter::AuditedRow<OrderHeaderRepository::Row>>
OrderHeaderRepository::convertRowToRow(
    const QVector<QString>& parts,
    CsvImporter::FileContext& ctx)
{
    if (parts.size() < 9) {
        ctx.addError(ctx.currentLineNumber(),
                     "⚠️ Kevés mező (9 szükséges)");
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
        parts[8].trimmed()
    };

    return CsvImporter::AuditedRow<Row>{ ctx.currentLineNumber(), r };
}

// ------------------------------------------------------------
// Validate Phase
// ------------------------------------------------------------
QVector<CsvImporter::RowError>
OrderHeaderRepository::validateRow(const Row& row, int lineNumber)
{
    QVector<CsvImporter::RowError> errors;

    if (row.idStr.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó id" });

    if (row.customerName.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó customerName" });

    return errors;
}

// ------------------------------------------------------------
// Build Phase
// ------------------------------------------------------------
std::optional<OrderHeader>
OrderHeaderRepository::buildFromRow(
    const Row& row,
    CsvImporter::FileContext& ctx)
{
    auto errs = validateRow(row, ctx.currentLineNumber());
    ctx.addErrors(errs);
    if (!errs.isEmpty())
        return std::nullopt;

    OrderHeader h;

    h.id = QUuid(row.idStr);
    if (h.id.isNull())
        h.id = QUuid::createUuid();

    h.customerName = row.customerName;
    h.orderDate = QDate::fromString(row.orderDate, Qt::ISODate);
    h.defaultColor = row.defaultColor;
    h.defaultHandlerSide = row.defaultHandlerSide;
    h.defaultExternalPrefix = row.defaultExternalPrefix;
    h.note = row.note;
    h.status = row.status;
    h.deadline = QDate::fromString(row.deadline, Qt::ISODate);

    // name = customerName (IdentifiableEntity)
    h.name = h.customerName;

    return h;
}

// ------------------------------------------------------------
// Assemble Phase
// ------------------------------------------------------------
QVector<CsvImporter::AuditedRow<OrderHeaderRepository::Row>>
OrderHeaderRepository::loadRows(CsvImporter::FileContext& ctx)
{
    return CsvImporter::readAndConvert<
        CsvImporter::AuditedRow<Row>
        >(ctx, convertRowToRow);
}

// ------------------------------------------------------------
// LOAD
// ------------------------------------------------------------
bool OrderHeaderRepository::load(QVector<OrderHeader>& out)
{
    auto path = FileNameHelper::instance().pathFor(FileKind::OrderHeaders, FileAccess::Read);
    CsvImporter::FileContext ctx("OrderHeader import", path);

    const auto rows = loadRows(ctx);

    QVector<OrderHeader> defs =
        CsvImporter::buildAll<Row, OrderHeader>(
            rows,
            buildFromRow,
            ctx
            );

    out = defs;

    zInfo(QString("📦 OrderHeaderRepository: %1 rekord betöltve").arg(defs.size()));
    return true;
}

// ------------------------------------------------------------
// SAVE
// ------------------------------------------------------------
bool OrderHeaderRepository::save(const QVector<OrderHeader>& data)
{
    auto path = FileNameHelper::instance().pathFor(FileKind::OrderHeaders, FileAccess::Write);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);

    out << "id;customerName;orderDate;defaultColor;defaultHandlerSide;"
           "defaultExternalPrefix;note;status;deadline\n";

    for (const auto& h : data)
        out << toCsvLine(h) << "\n";

    return true;
}

QString OrderHeaderRepository::toCsvLine(const OrderHeader& h)
{
    return QString("%1;%2;%3;%4;%5;%6;%7;%8;%9")
    .arg(h.id.toString(QUuid::WithoutBraces))
        .arg(h.customerName)
        .arg(h.orderDate.toString(Qt::ISODate))
        .arg(h.defaultColor)
        .arg(h.defaultHandlerSide)
        .arg(h.defaultExternalPrefix)
        .arg(h.note)
        .arg(h.status)
        .arg(h.deadline.toString(Qt::ISODate));
}
