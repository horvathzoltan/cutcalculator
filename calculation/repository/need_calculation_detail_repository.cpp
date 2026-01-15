#include "calculation/repository/need_calculation_detail_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"
#include <QFile>
#include <QTextStream>

// ------------------------------------------------------------
// Path helper
// ------------------------------------------------------------
QString NeedCalculationDetailRepository::filePath() {
    return FileNameHelper::instance().getNeedCalculationDetailCsvFile();
}

// ------------------------------------------------------------
// Stage 1: Convert
// ------------------------------------------------------------
std::optional<CsvImporter::AuditedRow<NeedCalculationDetailRepository::Row>>
NeedCalculationDetailRepository::convertRowToRow(
    const QVector<QString>& parts,
    CsvImporter::FileContext& ctx)
{
    if (parts.size() < 3) {
        ctx.addError(ctx.currentLineNumber(),
                     "⚠️ Kevés mező (3 szükséges: needCalculationBarcode;materialBarcode;formula)");
        return std::nullopt;
    }

    Row row {
        .needCalculationBarcode = parts[0].trimmed(),
        .materialBarcode        = parts[1].trimmed(),
        .formula                = parts[2].trimmed()
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
NeedCalculationDetailRepository::validateRow(const Row& row, int lineNumber)
{
    QVector<CsvImporter::RowError> errors;

    if (row.needCalculationBarcode.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó needCalculationBarcode", row.needCalculationBarcode, row.formula });

    if (row.materialBarcode.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó materialBarcode", row.materialBarcode, row.formula });

    if (row.formula.isEmpty())
        errors.append({ lineNumber, "⚠️ Hiányzó formula", row.needCalculationBarcode, row.formula });

    return errors;
}

// ------------------------------------------------------------
// Stage 2: Build (barcode → UUID feloldás)
// ------------------------------------------------------------
std::optional<NeedCalculationDetail>
NeedCalculationDetailRepository::buildFromRow(
    const Row& row,
    CsvImporter::FileContext& ctx)
{
    auto rowErrors = validateRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);
    if (!rowErrors.isEmpty())
        return std::nullopt;

    // 🔥 Reláció feloldása: NeedCalculation barcode → UUID
    const auto* calc =
        NeedCalculationRegistry::instance().findByName(row.needCalculationBarcode);

    if (!calc) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("⚠️ Ismeretlen NeedCalculation barcode: %1")
                         .arg(row.needCalculationBarcode),
                     row.needCalculationBarcode,
                     row.formula);
        return std::nullopt;
    }

    // 🔥 Reláció feloldása: Material barcode → UUID
    const auto* mat =
        MaterialRegistry::instance().findByBarcode(row.materialBarcode);

    if (!mat) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("⚠️ Ismeretlen Material barcode: %1")
                         .arg(row.materialBarcode),
                     row.materialBarcode,
                     row.formula);
        return std::nullopt;
    }

    NeedCalculationDetail d;
    d.id                = QUuid::createUuid();
    d.needCalculationId = calc->id;
    d.materialId        = mat->id;
    d.formula           = row.formula;

    return d;
}

// ------------------------------------------------------------
// Stage 3: Load & Assemble
// ------------------------------------------------------------
QVector<CsvImporter::AuditedRow<NeedCalculationDetailRepository::Row>>
NeedCalculationDetailRepository::loadRows(CsvImporter::FileContext& ctx)
{
    return CsvImporter::readAndConvert<
        CsvImporter::AuditedRow<Row>
        >(ctx, convertRowToRow);
}

// ------------------------------------------------------------
// Entry Point
// ------------------------------------------------------------
bool NeedCalculationDetailRepository::load(QVector<NeedCalculationDetail>& out)
{
    CsvImporter::FileContext ctx("NeedCalculationDetail import", filePath());

    const auto rows = loadRows(ctx);

    QVector<NeedCalculationDetail> defs =
        CsvImporter::buildAll<Row, NeedCalculationDetail>(
            rows,
            buildFromRow,
            ctx
            );

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a NeedCalculationDetail import során (%1)")
                     .arg(ctx.errorsSize()));
    }

    out = defs;

    zInfo(QString("📊 NeedCalculationDetailRepository: %1 rekord betöltve")
              .arg(defs.size()));

    return !defs.isEmpty();
}

// ------------------------------------------------------------
// SAVE (emberi kulcsokkal)
// ------------------------------------------------------------
bool NeedCalculationDetailRepository::save(const QVector<NeedCalculationDetail>& data)
{
    QFile file(filePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        FileHelper::logFileError(file, "CSV SAVE", QIODevice::WriteOnly);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "needCalculationBarcode;materialBarcode;formula\n";

    for (const auto& d : data) {
        const auto* calc = NeedCalculationRegistry::instance().findById(d.needCalculationId);
        const auto* mat  = MaterialRegistry::instance().findById(d.materialId);

        if (calc && mat) {
            out << calc->name << ";" << mat->barcode << ";" << d.formula << "\n";
        }
    }

    zInfo(QString("💾 NeedCalculationDetail saved: %1 sor").arg(data.size()));
    return true;
}
