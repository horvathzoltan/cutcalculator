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
std::optional<CsvImporter::AuditedRow<ProductRepository::ProductRow>>
ProductRepository::convertRowToProductRow(const QVector<QString>& parts,
                                          CsvImporter::FileContext& ctx) {
    if (parts.size() < 3) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Kevés mező (legalább 3)");
        return std::nullopt;
    }

    ProductRow row {
        .barcode       = parts[0].trimmed(),
        .parentBarcode = parts[1].trimmed(),
        .name          = parts[2].trimmed()
    };

    return CsvImporter::AuditedRow<ProductRow>{
        ctx.currentLineNumber(),
        row
    };
}

// --- Stage 2: Build ---
std::optional<ProductMaster>
ProductRepository::buildProductFromRow(const ProductRow& row,
                                       CsvImporter::FileContext& ctx) {
    auto rowErrors = validateProductRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);

    if (!rowErrors.isEmpty()) {
        return std::nullopt;
    }

    ProductMaster def;
    def.id = QUuid::createUuid();  // Hunglish: belső GUID generálás
    def.name = row.name;
    def.barcode = row.barcode;

    // Parent barcode → lookup → parentId
    if (row.parentBarcode.isEmpty()) {
        def.parentId = QUuid(); // root
    } else {
        if (auto* parent = ProductRegistry::instance().findByBarcode(row.parentBarcode)) {
            def.parentId = parent->id;
        } else {
            def.parentId = QUuid(); // fallback root
            zWarning(QString("⚠️ Ismeretlen parentBarcode → fallback root (sor %1) [%2 → %3]")
                         .arg(ctx.currentLineNumber())
                         .arg(row.barcode, row.parentBarcode));
        }
    }

    return def;
}


// --- Stage 2.5: Validate ---
QVector<CsvImporter::RowError>
ProductRepository::validateProductRow(const ProductRow& row, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    if (row.barcode.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó barcode", row.barcode, row.name});

    if (row.name.isEmpty())
        errors.append({lineNumber, "⚠️ Hiányzó name", row.barcode, row.name});

    return errors;
}


// --- Stage 3: Load & Assemble ---
QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow>>
ProductRepository::loadProductRows(CsvImporter::FileContext& ctx) {
    return CsvImporter::readAndConvert<CsvImporter::AuditedRow<ProductRepository::ProductRow>>(ctx, convertRowToProductRow);
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

    const auto rows = loadProductRows(ctx);

    // 🔍 Validáció a buildAll előtt
    validateProductRows(rows, ctx);

    // Első fázis: buildAll → minden ProductMaster id+barcode+name
    // Domain objektumok építése
    QVector<ProductMaster> defs =
        CsvImporter::buildAll<ProductRow, ProductMaster>(
            rows,
            buildProductFromRow,
            ctx
        );


    // Második fázis: parentBarcode → parentId feloldás
    resolveParents(defs, rows, ctx);

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a Product import során (%1)").arg(ctx.errorsSize()));
    }

    registry.setData(defs);
    zInfo(QString("📊 ProductRepository: %1 terméktípus betöltve").arg(defs.size()));
    return !defs.isEmpty();
}

// --- Új helper: validáció a buildAll előtt ---
void ProductRepository::validateProductRows(const QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow>>& rows,
                                CsvImporter::FileContext& ctx) {
    QSet<QString> seenBarcodes;

    // Első kör: saját barcode validáció
    for (const auto& audited : rows) {
        const auto& row = audited.row;
        int line = audited.rawLineNumber;

        if (row.barcode.isEmpty()) {
            ctx.addError(line, "⚠️ Hiányzó barcode", row.barcode, row.name);
        } else {
            if (seenBarcodes.contains(row.barcode)) {
                ctx.addError(line, "⚠️ Duplikált barcode", row.barcode, row.name);
            }
            seenBarcodes.insert(row.barcode);
        }

        if (row.name.isEmpty()) {
            ctx.addError(line, "⚠️ Hiányzó name", row.barcode, row.name);
        }
    }

    // Második kör: parentBarcode validáció
    for (const auto& audited : rows) {
        const auto& row = audited.row;
        int line = audited.rawLineNumber;

        if (!row.parentBarcode.isEmpty() && !seenBarcodes.contains(row.parentBarcode)) {
            ctx.addError(line,
                         "⚠️ Parent barcode nem található",
                         row.barcode,
                         row.parentBarcode);
        }
    }
}

/// Kétfázisú parent feloldás – barcode → GUID
void ProductRepository::resolveParents(QVector<ProductMaster>& defs,
                           const QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow>>& rows,
                           CsvImporter::FileContext& ctx) {
    // építs barcode→id mapet
    QHash<QString, QUuid> barcodeToId;
    for (const auto& pm : defs) {
        barcodeToId.insert(pm.barcode, pm.id);
    }

    // végigmegyünk a sorokon, és beállítjuk a parentId-t
    for (int i = 0; i < defs.size(); ++i) {
        const auto& row = rows[i].row;
        if (!row.parentBarcode.isEmpty()) {
            if (barcodeToId.contains(row.parentBarcode)) {
                defs[i].parentId = barcodeToId.value(row.parentBarcode);
            } else {
                defs[i].parentId = QUuid(); // fallback root
                zWarning(QString("⚠️ Parent barcode not found → fallback root [%2 → %3]")
                             .arg(defs[i].barcode, row.parentBarcode));
            }
        }
    }
}


bool ProductRepository::saveToCSV(const QVector<ProductMaster>& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        zWarning(QString("❌ Nem sikerült megnyitni a fájlt írásra: %1").arg(path));
        return false;
    }

    QTextStream out(&file);
    // Fejléc
    out << "barcode,parentBarcode,name\n";

    for (const auto& pm : data) {
        out << pm.barcode << ",";
        if (pm.parentId.isNull()) {
            out << ",";
        } else {
            // parent barcode lookup
            if (auto* parent = ProductRegistry::instance().findById(pm.parentId)) {
                out << parent->barcode << ",";
            } else {
                out << ",";
            }
        }
        out << pm.name << "\n";
    }

    zInfo(QString("💾 ProductRepository: %1 terméktípus mentve → %2")
              .arg(data.size()).arg(path));
    return true;
}

