#include "calculation/repository/need_calculation_detail_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"
#include <QFile>
#include <QTextStream>

// ------------------------------------------------------------
// Path helper
// ------------------------------------------------------------
// QString NeedCalculationDetailRepository::filePath() {
//     return FileNameHelper::instance().getNeedCalculationDetailCsvFile();
// }

// ------------------------------------------------------------
// Stage 1: Convert
// ------------------------------------------------------------
std::optional<CsvImporter::AuditedRow<NeedCalculationDetailRepository::Row>>
NeedCalculationDetailRepository::convertRowToRow(
    const QVector<QString>& parts,
    CsvImporter::FileContext& ctx)
{
    if (parts.size() < 4) {
        ctx.addError(ctx.currentLineNumber(),
                     "⚠️ Kevés mező (4 szükséges: productBarcode;modeName;materialBarcode;formula)");
        return std::nullopt;
    }

    Row row {
        .productBarcode  = parts[0].trimmed(),
        .modeName        = parts[1].trimmed(),
        .materialBarcode = parts[2].trimmed(),
        .formula         = parts[3].trimmed()
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

    if (row.productBarcode.isEmpty()) {
        errors.append({
            lineNumber,
            QString("⚠️ Hiányzó productBarcode (mode=%1, material=%2)")
                .arg(row.modeName, row.materialBarcode),
            row.productBarcode,
            row.formula
        });
    }

    if (row.modeName.isEmpty()) {
        errors.append({
            lineNumber,
            QString("⚠️ Hiányzó modeName (product=%1, material=%2)")
                .arg(row.productBarcode, row.materialBarcode),
            row.modeName,
            row.formula
        });
    }

    if (row.materialBarcode.isEmpty()) {
        errors.append({
            lineNumber,
            QString("⚠️ Hiányzó materialBarcode (product=%1, mode=%2)")
                .arg(row.productBarcode, row.modeName),
            row.materialBarcode,
            row.formula
        });
    }

    return errors;
}

// ------------------------------------------------------------
// Stage 2: Build (barcode → UUID feloldás)
// ------------------------------------------------------------
// std::optional<NeedCalculationDetail>
// NeedCalculationDetailRepository::buildFromRow(
//     const Row& row,
//     CsvImporter::FileContext& ctx)
// {
//     auto rowErrors = validateRow(row, ctx.currentLineNumber());
//     ctx.addErrors(rowErrors);
//     if (!rowErrors.isEmpty())
//         return std::nullopt;

//     // 1) Product lookup
//     const auto* product =
//         ProductRegistry::instance().findByBarcode(row.productBarcode);

//     if (!product) {
//         ctx.addError(ctx.currentLineNumber(),
//                      QString("⚠️ Ismeretlen productBarcode: %1")
//                          .arg(row.productBarcode),
//                      row.productBarcode,
//                      row.formula);
//         return std::nullopt;
//     }

//     // 2) NeedCalculation lookup (productId + modeName)
//     const auto* calc =
//         NeedCalculationRegistry::instance()
//             .findByProductAndName(product->id, row.modeName);

//     if (!calc) {
//         ctx.addError(ctx.currentLineNumber(),
//                      QString("⚠️ Ismeretlen NeedCalculation: %1 / %2")
//                          .arg(row.productBarcode, row.modeName),
//                      row.modeName,
//                      row.formula);
//         return std::nullopt;
//     }

//     if(calc->productId != product->id) {
//         ctx.addError(ctx.currentLineNumber(),
//                      QString("⚠️ NeedCalculation nem illeszkedik a Product-hoz: %1 / %2")
//                          .arg(row.productBarcode, row.modeName),
//                      row.modeName,
//                      row.formula);
//         return std::nullopt;
//     }

//     // 3) Material lookup
//     const auto* mat =
//         MaterialRegistry::instance().findByBarcode(row.materialBarcode);

//     if (!mat) {
//         ctx.addError(ctx.currentLineNumber(),
//                      QString("⚠️ Ismeretlen Material barcode: %1")
//                          .arg(row.materialBarcode),
//                      row.materialBarcode,
//                      row.formula);
//         return std::nullopt;
//     }

//     // fallback: derive kind from formula if CSV has no kind column
//     NeedCalculationDetail::DetailKind kind =
//         row.formula.startsWith("fixed:")
//             ? NeedCalculationDetail::DetailKind::Kitting
//             : NeedCalculationDetail::DetailKind::Cutting;


//     NeedCalculationDetail d;
//     d.id                = QUuid::createUuid();
//     d.needCalculationId = calc->id;
//     d.materialId        = mat->id;
//     d.formula           = row.formula;
//     d.kind = kind;

//     return d;
// }
std::optional<NeedCalculationDetail>
NeedCalculationDetailRepository::buildFromRow(
    const Row& row,
    CsvImporter::FileContext& ctx)
{
    // 1) Validáció (formula már nem kötelező – p1)
    auto rowErrors = validateRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);
    if (!rowErrors.isEmpty())
        return std::nullopt;

    // 2) Product lookup (barcode → UUID)
    const auto* product =
        ProductRegistry::instance().findByBarcode(row.productBarcode);

    if (!product) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("⚠️ Ismeretlen productBarcode: '%1' (mode=%2, material=%3)")
                         .arg(row.productBarcode, row.modeName, row.materialBarcode),
                     row.productBarcode,
                     row.formula);
        return std::nullopt;
    }

    // 3) NeedCalculation lookup (productId + modeName)
    const auto* calc =
        NeedCalculationRegistry::instance()
            .findByProductAndName(product->id, row.modeName);

    if (!calc) {
        ctx.addError(ctx.currentLineNumber(),
                      QString("⚠️ Ismeretlen számítási mód: '%1' (product=%2, material=%3)")
                          .arg(row.modeName, row.productBarcode,  row.materialBarcode),
                      row.modeName,
                      row.formula);
        return std::nullopt;
    }

    // Biztonsági ellenőrzés (ritka, de fontos)
    if (calc->productId != product->id) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("⚠️ NeedCalculation nem illeszkedik a Product‑hoz: product='%1', mode='%2'")
                         .arg(row.productBarcode, row.modeName),
                     row.modeName,
                     row.formula);
        return std::nullopt;
    }

    // 4) Material lookup (barcode → UUID)
    const auto* mat =
        MaterialRegistry::instance().findByBarcode(row.materialBarcode);

    if (!mat) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("⚠️ Ismeretlen Material barcode: '%1' (product=%2, mode=%3)")
                         .arg(row.materialBarcode, row.productBarcode, row.modeName),
                     row.materialBarcode,
                     row.formula);
        return std::nullopt;
    }

    // 5) Kind meghatározása (formula alapján)
    NeedCalculationDetail::DetailKind kind =
        (mat && mat->cuttingMode == CuttingMode::Piece)
            ? NeedCalculationDetail::DetailKind::Kitting
            : NeedCalculationDetail::DetailKind::Cutting;

    // 6) NeedCalculationDetail összeállítása
    NeedCalculationDetail d;
    d.id                = QUuid::createUuid();
    d.needCalculationId = calc->id;
    d.materialId        = mat->id;
    d.formula           = row.formula;
    d.kind              = kind;

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
    auto filePath = FileNameHelper::instance().pathFor(FileKind::NeedCalculationDetails, FileAccess::Read);
    if(filePath.isEmpty()) {
        zWarning("⚠️ Nem elérhető fájlútvonal a NeedCalculationDetail CSV betöltéséhez.");
        return false;
    }

    CsvImporter::FileContext ctx("NeedCalculationDetail import", filePath);

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

    if (!ctx.fileError().isEmpty()) {
        out.clear();
        return false;
    }

    out = defs;

    zInfo(QString("📊 NeedCalculationDetailRepository: %1 rekord betöltve")
              .arg(defs.size()));

    return true;

}

// ------------------------------------------------------------
// SAVE (emberi kulcsokkal)
// ------------------------------------------------------------
bool NeedCalculationDetailRepository::save(const QVector<NeedCalculationDetail>& data)
{
    auto filePath = FileNameHelper::instance().pathFor(FileKind::NeedCalculationDetails, FileAccess::Write);
    if(filePath.isEmpty()) {
        zWarning("⚠️ Nem elérhető fájlútvonal a NeedCalculationDetail CSV mentéséhez.");
        return false;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        FileHelper::logFileError(file, "CSV SAVE", QIODevice::WriteOnly);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "productBarcode;modeName;materialBarcode;formula\n";

    for (const auto& d : data) {
        const auto* calc = NeedCalculationRegistry::instance().findById(d.needCalculationId);
        const auto* mat  = MaterialRegistry::instance().findById(d.materialId);

        if (!calc || !mat)
            continue;

        const auto* product =
            ProductRegistry::instance().findById(calc->productId);

        if (!product)
            continue;

        out << product->barcode << ";"
            << calc->name << ";"
            << mat->barcode << ";"
            << d.formula << "\n";
    }

    zInfo(QString("💾 NeedCalculationDetail saved: %1 sor").arg(data.size()));
    return true;
}
