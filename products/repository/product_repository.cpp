// products/repository/product_repository.cpp
#include <QUuid>
#include <QFile>
#include <QTextStream>

#include <barcodes/validator//barcode_validator.h>

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
// A ProductMaster objektum felépítése registry-hívás nélkül.
// A parentBarcode feloldása NEM itt történik (resolveParents végzi).
// A barcode-regisztráció itt történik a BarcodeValidatoron keresztül
// (globális ledger-kapu, auditált életút-kezelés).

std::optional<ProductMaster>
ProductRepository::buildProductFromRow(const ProductRow& row,
                                       CsvImporter::FileContext& ctx) {
    // Tartalmi validáció (barcode-regisztráció NEM itt történik)
    auto rowErrors = validateProductRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);
    if (!rowErrors.isEmpty()) return std::nullopt;

    ProductMaster def;
    def.id = QUuid::createUuid();
    def.name = row.name;
    def.barcode = row.barcode;
    def.parentId = QUuid(); // ne állíts itt parentet

    if (!BarcodeValidator::checkAndRegister(row.barcode, "Product", def.id, def.name, ctx))
        return std::nullopt;

    return def;

}


CsvImporter::RowError ProductRepository::makeError(int lineNumber,
                                                    const QString& message,
                                                    const ProductRepository::ProductRow& row) {
    return { lineNumber, message, row.barcode, row.name };
}

// --- Stage 2.5: Validate ---
// Barcode-validáció egységesen a BarcodeCollisionHelper-rel (registry-hívás nélkül)
QVector<CsvImporter::RowError>
ProductRepository::validateProductRow(const ProductRow& row, int lineNumber) {
    QVector<CsvImporter::RowError> errors;

    if (row.barcode.isEmpty()){
        errors.append({lineNumber, "⚠️ Hiányzó barcode", row.barcode, row.name});
    } else {
        if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
                "Product",
                BarcodeCollisionHelper::RowInfo{ row.barcode, row.name, std::nullopt },
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
// CSV → ProductMaster betöltés (barcode-regisztráció itt történik a BarcodeValidatoron keresztül)
bool ProductRepository::load(QVector<ProductMaster>& out)
{
    auto& helper = FileNameHelper::instance();

    const QString fn = helper.pathFor(FileKind::Products, FileAccess::Read);;
    CsvImporter::FileContext ctx("Product import", fn);

    const auto rows = loadProductRows(ctx);

    QVector<ProductMaster> defs =
        CsvImporter::buildAll<ProductRow, ProductMaster>(
            rows,
            buildProductFromRow,
            ctx
            );

    resolveParents(defs, rows, ctx);

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a product import során (%1)")
                     .arg(ctx.errorsSize()));
    }

    if (!ctx.fileError().isEmpty()) {
        out.clear();
        return false;
    }

    out = defs;

    zInfo(QString("📦 ProductRepository: %1 termék betöltve").arg(defs.size()));
    return true;
}

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
            def.parentId = QUuid();
            ctx.addError(-1, "Missing CSV row for barcode", def.barcode, def.name);

            zWarning(QString("ProductRepository::resolveParents – missing CSV row (barcode=%1)")
                         .arg(def.barcode));

            orphanSummaries.append(
                QString("Line ?: %1 → missing CSV row").arg(def.barcode));

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

            const QString msg = QString("Parent barcode nem található: %1").arg(row.parentBarcode);
            ctx.addError(line, "Parent barcode not found", row.barcode, row.name);

            zWarning(QString("ProductRepository::resolveParents – parent not found (line %1): %2 → %3")
                         .arg(line).arg(def.barcode, row.parentBarcode));

            const QString summary =
                QString("Line %1: %2 → %3 (parent not found)")
                    .arg(line)
                    .arg(def.barcode, row.parentBarcode);
            orphanSummaries.append(summary);
        }
    }

    if(IS_VERBOSE(ProductRepository)){
        // verbose diagnostic output
        for (const auto& pm : defs) {
            zInfo(QString("ProductRepository::resolveParents – result: %1 barcode=%2 parentId=%3")
                      .arg(pm.name, pm.barcode, pm.parentId.toString(QUuid::WithoutBraces)));
        }
    }

    // orphan összefoglaló blokk
    if (!orphanSummaries.isEmpty()) {
        zWarning("ProductRepository::resolveParents – orphan summary:");
        for (const auto& s : orphanSummaries) {
            zWarning(s);
        }
    }
}



/**
 * @brief Egy ProductMaster objektumot CSV-sorrá alakít.
 *
 * A CSV export egységes formátumát biztosítja:
 *     barcode,parentBarcode,name
 *
 * - A parentBarcode mező a parentId alapján kerül feloldásra.
 * - Ha a parentId üres vagy a parent nem található, a mező üres marad.
 *
 * Ezt a függvényt használja:
 *   - a saveToCSV() exportáláskor
 *   - a unit tesztek a CSV tartalom ellenőrzésére
 *
 * @param pm A termék definíciója.
 * @return A CSV-sor, fejléc nélkül.
 */

QString ProductRepository::toCsvLine(const ProductMaster& pm)
{
    QString parentBarcode;

    if (!pm.parentId.isNull()) {
        if (auto* parent = ProductRegistry::instance().findById(pm.parentId)) {
            parentBarcode = parent->barcode;
        }
    }

    const QString X = ",";
    QString e = pm.barcode % X % parentBarcode % X % pm.name;

    return e;
}

/**
 * @brief A teljes ProductMaster lista CSV fájlba írása.
 *
 * A fájl formátuma:
 *     barcode,parentBarcode,name
 * majd minden sorra a toCsvLine() által generált érték kerül.
 *
 * A függvény:
 *   - felülírja a megadott fájlt,
 *   - UTF-8 kódolással ír,
 *   - minden CRUD művelet után automatikusan meghívódik
 *     a ProductRegistry::persist() workflow részeként.
 *
 * A tesztek a generált CSV-t QStringList-ként olvassák vissza,
 * és a toCsvLine() által generált sorokkal hasonlítják össze.
 *
 * @param data A registry aktuális tartalma.
 * @param path A célfájl elérési útja.
 * @return true siker esetén, false ha a fájl nem nyitható meg.
 */

bool ProductRepository::saveToCSV(const QVector<ProductMaster>& data) {
    auto& helper = FileNameHelper::instance();

    const QString path = helper.pathFor(FileKind::Products, FileAccess::Write);
    if (path.isEmpty())
        return false;

    QFile file(path);
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (!file.open(mode)) {
        FileHelper::logFileError(file, "CSV SAVE", mode);
        return false;
    }

    QTextStream out(&file);
    out << "barcode,parentBarcode,name\n";     // Fejléc

    for (const auto& pm : data) {
        out << ProductRepository::toCsvLine(pm) << "\n";
    }

    zInfo(QString("💾 ProductRepository: %1 terméktípus mentve → %2")
              .arg(data.size()).arg(path));
    return true;
}

