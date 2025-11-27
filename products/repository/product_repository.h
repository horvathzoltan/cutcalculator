// products/repository/product_repository.h
#pragma once
#include "products/model/product_definition.h"
#include "common/csv/filecontext.h"
#include "products/registry/product_registry.h"
#include <QVector>
#include <QString>
#include <optional>

/**
 * ProductRepository
 *
 * Three Phase Import minta alapján tölti be a terméktípusokat CSV-ből.
 * Fázisok:
 *  - Convert: CSV sor → ProductRow
 *  - Build: ProductRow → ProductDefinition
 *  - Assemble: ProductDefinition → ProductRegistry
 */
class ProductRepository {
public:
    static bool loadFromCSV(ProductRegistry& registry);

private:
    /// Nyers CSV sor → típusos Row
    struct ProductRow {
        QString idStr;
        QString parentIdStr;
        QString name;
        QString barcode;
    };

    // --- Stage 1: Convert ---
    static std::optional<ProductRow> convertRowToProductRow(const QVector<QString>& parts,
                                                            CsvImporter::FileContext& ctx);

    // --- Stage 2: Build ---
    static std::optional<ProductDefinition> buildProductFromRow(const ProductRow& row,
                                                                CsvImporter::FileContext& ctx);

    // --- Stage 3: Load & Assemble ---
    static QVector<ProductRow> loadProductRows(CsvImporter::FileContext& ctx);
};
