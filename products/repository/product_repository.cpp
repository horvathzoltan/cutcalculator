// products/repository/product_repository.cpp
#include <QUuid>
#include <QFile>
#include <QTextStream>

#include "products/repository/product_repository.h"
#include "products/registry/product_registry.h"
#include "common/csv/csvimporter.h"
#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"

// --- Stage 1: Convert ---
std::optional<ProductRepository::ProductRow>
ProductRepository::convertRowToProductRow(const QVector<QString>& parts,
                                          CsvImporter::FileContext& ctx) {
    if (parts.size() < 4) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Kevés mező (legalább 4)");
        return std::nullopt;
    }

    ProductRow row {
        .idStr       = parts[0].trimmed(),
        .parentIdStr = parts[1].trimmed(),
        .name        = parts[2].trimmed(),
        .barcode     = parts[3].trimmed()
    };

    return row;
}

// --- Stage 2: Build ---
std::optional<ProductMaster>
ProductRepository::buildProductFromRow(const ProductRow& row,
                                       CsvImporter::FileContext& ctx) {
    auto rowErrors = validateProductRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);

    if (!rowErrors.isEmpty()) {
        // Ha kritikus hiba van, ne építsük meg az objektumot
        return std::nullopt;
    }

    ProductMaster def;
    def.id = QUuid(row.idStr);

    if (row.parentIdStr.isEmpty() || row.parentIdStr == "{}" ||
        row.parentIdStr == "00000000-0000-0000-0000-000000000000") {
        def.parentId = QUuid(); // gyökér
    } else {
        QUuid pid(row.parentIdStr);
        if (!pid.isNull()) {
            def.parentId = pid;
        } else {
            zWarning(QString("⚠️ Érvénytelen parentId → fallback root (sor %1)").arg(ctx.currentLineNumber()));
            def.parentId = QUuid(); // fallback root
        }
    }


    def.name = row.name;
    def.barcode = row.barcode;

    return def;
}


// --- Stage 2.5: Validate ---
QVector<CsvImporter::RowError>
ProductRepository::validateProductRow(const ProductRow& row, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    if (row.idStr.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó id"});
    else {
        QUuid id(row.idStr);
        if (id.isNull())
            errors.append({lineNumber, "⚠️ Érvénytelen UUID"});
    }

    if (row.name.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó name"});

    if (row.barcode.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó barcode"});

    if (!row.parentIdStr.isEmpty()) {
        QUuid pid(row.parentIdStr);
        if (pid.isNull())
            errors.append({lineNumber, "⚠️ Érvénytelen parentId UUID"});
    }

    return errors;
}


// --- Stage 3: Load & Assemble ---
QVector<ProductRepository::ProductRow>
ProductRepository::loadProductRows(CsvImporter::FileContext& ctx) {
    return CsvImporter::readAndConvert<ProductRow>(ctx, convertRowToProductRow);
}

// --- Entry Point ---
bool ProductRepository::loadFromCSV(ProductRegistry& registry) {
    const auto& helper = FileNameHelper::instance();
    if (!helper.isInitialized()) {
        zWarning("❌ A FileNameHelper nincs inicializálva.");
        return false;
    }

    const QString path = helper.getProductCsvFile(); // products.csv
    CsvImporter::FileContext ctx("Product import", path);

    const QVector<ProductRepository::ProductRow> rows = loadProductRows(ctx);

    const QVector<ProductMaster> defs =
        CsvImporter::buildAll<ProductRow, ProductMaster>(
        rows, buildProductFromRow, ctx);

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a Product import során (%1)").arg(ctx.errorsSize()));
    }

    registry.setData(defs);
    zInfo(QString("📊 ProductRepository: %1 terméktípus betöltve").arg(defs.size()));
    return !defs.isEmpty();
}

/*
id,parentId,name,barcode
{11111111-1111-1111-1111-111111111111},,Roletta,ROL
{22222222-2222-2222-2222-222222222222},{11111111-1111-1111-1111-111111111111},Rugós,RUG
{33333333-3333-3333-3333-333333333333},{22222222-2222-2222-2222-222222222222},Tetőtéri,TET
{44444444-4444-4444-4444-444444444444},{22222222-2222-2222-2222-222222222222},Normál,NOR
{55555555-5555-5555-5555-555555555555},{11111111-1111-1111-1111-111111111111},Motoros,MOT
{66666666-6666-6666-6666-666666666666},,Sávroló,SAV
{77777777-7777-7777-7777-777777777777},{66666666-6666-6666-6666-666666666666},Rugós,SAVRUG
{88888888-8888-8888-8888-888888888888},{66666666-6666-6666-6666-666666666666},Motoros,SAVMOT
*/

bool ProductRepository::saveToCSV(const QVector<ProductMaster>& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        zWarning(QString("❌ Nem sikerült megnyitni a fájlt írásra: %1").arg(path));
        return false;
    }

    QTextStream out(&file);
    // Fejléc
    out << "id,parentId,name,barcode\n";

    for (const auto& pm : data) {
        out << pm.id.toString(QUuid::WithBraces) << ",";
        if (pm.parentId.isNull()) {
            out << ",";
        } else {
            out << pm.parentId.toString(QUuid::WithBraces) << ",";
        }
        out << pm.name << ",";
        out << pm.barcode << "\n";
    }

    zInfo(QString("💾 ProductRepository: %1 terméktípus mentve → %2")
              .arg(data.size()).arg(path));
    return true;
}

