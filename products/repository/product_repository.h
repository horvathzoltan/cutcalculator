// products/repository/product_repository.h
#pragma once
#include "common/csv/csvimporter.h"
#include "products/model/product_master.h"
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
    static bool load(QVector<ProductMaster>& out);
    //static bool loadFromCSV(ProductRegistry& registry);
    static bool saveToCSV(const QVector<ProductMaster>& data, const QString& path);

private:
    /// Nyers CSV sor → típusos Row
    struct ProductRow {
        QString barcode;
        QString parentBarcode;
        QString name;
    };

    // --- Stage 1: Convert ---
    static std::optional<CsvImporter::AuditedRow<ProductRow>> convertRowToProductRow(const QVector<QString>& parts,
                                                            CsvImporter::FileContext& ctx);

    // --- Stage 2: Build ---
    static std::optional<ProductMaster> buildProductFromRow(const ProductRow& row,
                                                                CsvImporter::FileContext& ctx);
    // --- Stage 2.5: Validate ---
    static QVector<CsvImporter::RowError> validateProductRow(const ProductRow& row, int lineNumber);

    // --- Stage 3: Load & Assemble ---
    static QVector<CsvImporter::AuditedRow<ProductRow>> loadProductRows(CsvImporter::FileContext& ctx);
    static void resolveParents(QVector<ProductMaster> &defs, const QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow> > &rows, CsvImporter::FileContext &ctx);
    //static void validateProductRows(const QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow> > &rows, CsvImporter::FileContext &ctx);
    static CsvImporter::RowError makeError(int lineNumber, const QString &message, const ProductRepository::ProductRow &row);
};
