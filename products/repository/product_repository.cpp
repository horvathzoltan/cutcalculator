// products/repository/product_repository.cpp
#include "products/repository/product_repository.h"
#include "products/registry/product_registry.h"
#include "common/csv/csvimporter.h"
#include "common/logger/logger.h"
#include <QUuid>
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
std::optional<ProductDefinition>
ProductRepository::buildProductFromRow(const ProductRow& row,
                                       CsvImporter::FileContext& ctx) {
    if (row.idStr.isEmpty() || row.name.isEmpty()) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Hiányzó id vagy name");
        return std::nullopt;
    }

    ProductDefinition def;
    def.id = QUuid(row.idStr);
    def.parentId = row.parentIdStr.isEmpty() ? QUuid() : QUuid(row.parentIdStr);
    def.name = row.name;
    def.barcode = row.barcode;

    return def;
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

    const QVector<ProductDefinition> defs =
        CsvImporter::buildAll<ProductRow, ProductDefinition>(
        rows, buildProductFromRow, ctx);

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a Product import során (%1)").arg(ctx.errorsSize()));
    }

    registry.setData(defs);
    zInfo(QString("📊 ProductRepository: %1 terméktípus betöltve").arg(defs.size()));
    return !defs.isEmpty();
}
