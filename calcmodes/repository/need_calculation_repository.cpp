#include "calcmodes/repository/need_calculation_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"
#include <QFile>
#include <QTextStream>

// ------------------------------------------------------------
// Path helper
// ------------------------------------------------------------
QString NeedCalculationRepository::filePath() {
    return FileNameHelper::instance().getNeedCalculationCsvFile();
}

// ------------------------------------------------------------
// Stage 1: Convert
// ------------------------------------------------------------
std::optional<CsvImporter::AuditedRow<NeedCalculationRepository::Row>>
NeedCalculationRepository::convertRowToRow(
    const QVector<QString>& parts,
    CsvImporter::FileContext& ctx)
{
    if (parts.size() < 2) {
        ctx.addError(ctx.currentLineNumber(),
                     "⚠️ Kevés mező (2 szükséges: productBarcode;modeName)");
        return std::nullopt;
    }

    Row row {
        .productBarcode = parts[0].trimmed(),
        .modeName       = parts[1].trimmed()
    };

    return CsvImporter::AuditedRow<Row>{
        ctx.currentLineNumber(),
        row
    };
}

// ------------------------------------------------------------
// Stage 2.5: Validate
// ------------------------------------------------------------
QVector<CsvImporter::RowError>
NeedCalculationRepository::validateRow(const Row& row, int lineNumber)
{
    QVector<CsvImporter::RowError> errors;

    if (row.productBarcode.isEmpty()) {
        errors.append({ lineNumber, "⚠️ Hiányzó productBarcode", row.productBarcode, row.modeName });
    }

    if (row.modeName.isEmpty()) {
        errors.append({ lineNumber, "⚠️ Hiányzó modeName", row.productBarcode, row.modeName });
    }

    return errors;
}

// ------------------------------------------------------------
// Stage 2: Build (barcode → UUID feloldás)
// ------------------------------------------------------------
std::optional<NeedCalculation>
NeedCalculationRepository::buildFromRow(const Row& row,
                                        CsvImporter::FileContext& ctx)
{
    auto rowErrors = validateRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);
    if (!rowErrors.isEmpty())
        return std::nullopt;

    // 🔥 Reláció feloldása: barcode → ProductMaster.id
    const auto* product =
        ProductRegistry::instance().findByBarcode(row.productBarcode);

    if (!product) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("⚠️ Ismeretlen productBarcode: %1")
                         .arg(row.productBarcode),
                     row.productBarcode,
                     row.modeName);
        return std::nullopt;
    }

    NeedCalculation nc;
    nc.id        = QUuid::createUuid();
    nc.productId = product->id;
    nc.name      = row.modeName;

    return nc;
}

// ------------------------------------------------------------
// Stage 3: Load & Assemble
// ------------------------------------------------------------
QVector<CsvImporter::AuditedRow<NeedCalculationRepository::Row>>
NeedCalculationRepository::loadRows(CsvImporter::FileContext& ctx)
{
    return CsvImporter::readAndConvert<
        CsvImporter::AuditedRow<Row>
        >(ctx, convertRowToRow);
}

// ------------------------------------------------------------
// Entry Point
// ------------------------------------------------------------
bool NeedCalculationRepository::load(QVector<NeedCalculation>& out)
{
    CsvImporter::FileContext ctx("NeedCalculation import", filePath());

    const auto rows = loadRows(ctx);

    QVector<NeedCalculation> defs =
        CsvImporter::buildAll<Row, NeedCalculation>(
            rows,
            buildFromRow,
            ctx
            );

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a NeedCalculation import során (%1)")
                     .arg(ctx.errorsSize()));
    }

    out = defs;

    zInfo(QString("📊 NeedCalculationRepository: %1 mód betöltve")
              .arg(defs.size()));

    return !defs.isEmpty();
}

// ------------------------------------------------------------
// SAVE (emberi kulcsokkal)
// ------------------------------------------------------------
bool NeedCalculationRepository::save(const QVector<NeedCalculation>& data)
{
    QFile file(filePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        FileHelper::logFileError(file, "CSV SAVE", QIODevice::WriteOnly);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "productBarcode;modeName\n";

    for (const auto& c : data) {
        if (auto* p = ProductRegistry::instance().findById(c.productId)) {
            out << p->barcode << ";" << c.name << "\n";
        }
    }

    zInfo(QString("💾 NeedCalculation saved: %1 sor").arg(data.size()));
    return true;
}
