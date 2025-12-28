// products/repository/product_repository.cpp
#include <QUuid>
#include <QFile>
#include <QTextStream>

#include "products/repository/product_repository.h"
#include "products/registry/product_registry.h"
#include "common/csv/csvimporter.h"
#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"
#include "barcodes/helpers/barcode_collision_helper.h"
#include "common/system/verbose_manager.h"

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
// std::optional<ProductMaster>
// ProductRepository::buildProductFromRow(const ProductRow& row,
//                                        CsvImporter::FileContext& ctx) {
//     auto rowErrors = validateProductRow(row, ctx.currentLineNumber());
//     ctx.addErrors(rowErrors);

//     if (!rowErrors.isEmpty()) {
//         return std::nullopt;
//     }

//     ProductMaster def;
//     def.id = QUuid::createUuid();  // Hunglish: belső GUID generálás
//     def.name = row.name;
//     def.barcode = row.barcode;

//     // Parent barcode → lookup → parentId
//     if (row.parentBarcode.isEmpty()) {
//         def.parentId = QUuid(); // root
//     } else {
//         if (auto* parent = ProductRegistry::instance().findByBarcode(row.parentBarcode)) {
//             def.parentId = parent->id;
//         } else {
//             def.parentId = QUuid(); // fallback root
//             zWarning(QString("⚠️ Ismeretlen parentBarcode → fallback root (sor %1) [%2 → %3]")
//                          .arg(ctx.currentLineNumber())
//                          .arg(row.barcode, row.parentBarcode));
//         }
//     }

//     return def;
// }

std::optional<ProductMaster>
ProductRepository::buildProductFromRow(const ProductRow& row,
                                       CsvImporter::FileContext& ctx) {
    auto rowErrors = validateProductRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);
    if (!rowErrors.isEmpty()) return std::nullopt;

    ProductMaster def;
    def.id = QUuid::createUuid();
    def.name = row.name;
    def.barcode = row.barcode;
    def.parentId = QUuid(); // ne állíts itt parentet
    return def;
}


CsvImporter::RowError ProductRepository::makeError(int lineNumber,
                                                    const QString& message,
                                                    const ProductRepository::ProductRow& row) {
    return { lineNumber, message, row.barcode, row.name };
}

// --- Stage 2.5: Validate ---
QVector<CsvImporter::RowError>
ProductRepository::validateProductRow(const ProductRow& row, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    if (row.barcode.isEmpty()){
        errors.append({lineNumber, "⚠️ Hiányzó barcode", row.barcode, row.name});
    } else {
        if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
                "Product",
                BarcodeCollisionHelper::RowInfo{ row.barcode, row.name },
                lineNumber))
        {
            errors.append(*err);
        }
    }

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
    const auto& fileNameHelper_instance = FileNameHelper::instance();
    if (!fileNameHelper_instance.isInitialized()) {
        zWarning("⚠️ A FileNameHelper nincs inicializálva.");
        return false;
    }

    const QString path = fileNameHelper_instance.getProductCsvFile(); // products.csv
    CsvImporter::FileContext ctx("Product import", path);

    const auto rows = loadProductRows(ctx);

    // 🔍 Validáció a buildAll előtt
    //validateProductRows(rows, ctx);

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
// void ProductRepository::validateProductRows(const QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow>>& rows,
//                                 CsvImporter::FileContext& ctx) {
//     //QSet<QString> seenBarcodes;

//     // Első kör: saját barcode validáció
//     for (const auto& audited : rows) {
//         const auto& row = audited.row;
//         int line = audited.rawLineNumber;

//         // if (row.barcode.isEmpty()) {
//         //     ctx.addError(line, "⚠️ Hiányzó barcode", row.barcode, row.name);
//         // } else {
//         //     if (seenBarcodes.contains(row.barcode)) {
//         //         ctx.addError(line, "⚠️ Duplikált barcode", row.barcode, row.name);
//         //     }
//         //     seenBarcodes.insert(row.barcode);
//         // }

//         if (row.name.isEmpty()) {
//             ctx.addError(line, "⚠️ Hiányzó name", row.barcode, row.name);
//         }
//     }

//     // Második kör: parentBarcode validáció
//     for (const auto& audited : rows) {
//         const auto& row = audited.row;
//         int line = audited.rawLineNumber;

//         if (!row.parentBarcode.isEmpty() && !seenBarcodes.contains(row.parentBarcode)) {
//             ctx.addError(line,
//                          "⚠️ Parent barcode nem található",
//                          row.barcode,
//                          row.parentBarcode);
//         }
//     }
// }

/// Kétfázisú parent feloldás – barcode → GUID
void ProductRepository::resolveParents(QVector<ProductMaster>& defs,
                                       const QVector<CsvImporter::AuditedRow<ProductRepository::ProductRow>>& rows,
                                       CsvImporter::FileContext& ctx) {
    // barcode → id
    QHash<QString, QUuid> barcodeToId;
    for (const auto& pm : defs) {
        barcodeToId.insert(pm.barcode, pm.id);
    }

    // barcode → eredeti AuditedRow (sorszámmal)
    QHash<QString, CsvImporter::AuditedRow<ProductRepository::ProductRow>> rowByBarcode;
    for (const auto& ar : rows) {
        rowByBarcode.insert(ar.row.barcode, ar);
    }

    // összefoglaló orphanokra
    QVector<QString> orphanSummaries;

    for (auto& def : defs) {
        const auto it = rowByBarcode.find(def.barcode);
        if (it == rowByBarcode.end()) {
            // védőháló: hiányzó CSV sor
            ctx.addError(-1, "⚠️ Hiányzik a CSV sor a barcode-hoz", def.barcode, def.name);
            def.parentId = QUuid();
            orphanSummaries.append(QString("Sor ?: %1 → hiányzó CSV sor").arg(def.barcode));
            continue;
        }
        const auto& audited = it.value();
        const auto& row     = audited.row;
        const int   line    = audited.rawLineNumber;

        if (row.parentBarcode.isEmpty()) {
            // tényleges root, nem orphan
            def.parentId = QUuid();
        } else if (barcodeToId.contains(row.parentBarcode)) {
            // parent sikeresen feloldva
            def.parentId = barcodeToId.value(row.parentBarcode);
        } else {
            // orphan → root + audit
            def.parentId = QUuid();

            const QString msg = QString("⚠️ Parent barcode nem található: %1").arg(row.parentBarcode);
            ctx.addError(line, msg, row.barcode, row.name);

            const QString summary = QString("Sor %1: %2 → %3 (parent nem található)")
                                        .arg(line)
                                        .arg(def.barcode, row.parentBarcode);
            zWarning(QString("⚠️ Parent barcode not found (sor %1): %2 → %3")
                         .arg(line).arg(def.barcode, row.parentBarcode));
            orphanSummaries.append(summary);
        }
    }

    if(IS_VERBOSE(ProductRepository)){
        // részletes feloldási log (diagnosztika)
        for (const auto& pm : defs) {
            zInfo(QString("resolveParents result: %1 barcode=%2 parentId=%3")
                      .arg(pm.name, pm.barcode, pm.parentId.toString(QUuid::WithoutBraces)));
        }
    }

    // orphan összefoglaló blokk
    if (!orphanSummaries.isEmpty()) {
        zWarning("— Orphan összefoglaló —");
        for (const auto& s : orphanSummaries) {
            zWarning(s);
        }
    }
}





bool ProductRepository::saveToCSV(const QVector<ProductMaster>& data, const QString& path) {
    QFile file(path);
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (!file.open(mode)) {
        //zWarning(QString("⚠️ Nem sikerült megnyitni a fájlt írásra: %1").arg(path));
        FileHelper::logFileError(file, "CSV SAVE", mode);
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

