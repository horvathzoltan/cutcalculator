#include "barcodes/repository/barcode_repository.h"
#include "common/utils/filename_helper.h"
#include "common/csv/csvimporter.h"
#include "common/logger/logger.h"
#include "common/logger/event_logger.h"
#include "common/system/verbose_manager.h"

#include <QFile>
#include <QTextStream>
#include <QUuid>

/**
 * CSV formátum – stabil szerződés audit és diff szempontból.
 *
 * Header (ajánlott, nem kötelező):
 *   code;entityType;entityId;introducedAt;retiredAt;status
 *
 * Hunglish:
 * - Dátumok: Qt::ISODate (YYYY-MM-DDTHH:MM:SS).
 * - retiredAt: üres, ha nincs nyugdíjazva.
 * - status: "Active" vagy "Retired".
 */

// --- Stage 1: Convert ---
std::optional<CsvImporter::AuditedRow<BarcodeRepository::BarcodeRow>>
BarcodeRepository::convertRowToBarcodeRow(const QVector<QString>& parts,
                                          CsvImporter::FileContext& ctx)
{
    // Most pontosan 4 mezőt várunk
    if (parts.size() < 4) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Kevés mező (4 szükséges)");
        return std::nullopt;
    } else if (parts.size() > 4) {
        ctx.addError(ctx.currentLineNumber(), "⚠️ Túl sok mező (4 szükséges)");
        return std::nullopt;
    }

    BarcodeRow row;
    row.code            = parts[0].trimmed();
    row.entityType      = parts[1].trimmed();
    row.introducedAtStr = parts[2].trimmed();
    row.retiredAtStr    = parts[3].trimmed();

    return CsvImporter::AuditedRow<BarcodeRow>{ ctx.currentLineNumber(), row };
}


CsvImporter::RowError
BarcodeRepository::makeError(int lineNumber, const QString& message, const BarcodeRepository::BarcodeRow& row) {
    // Auditbarát: a barcode és egy emberi kontextus (entityType) bekerül a hibába
    return { lineNumber, message, row.code, row.entityType };
}

// --- Stage 2.5: Validate ---
QVector<CsvImporter::RowError>
BarcodeRepository::validateBarcodeRow(const BarcodeRow& row, int lineNumber)
{
    QVector<CsvImporter::RowError> errors;

    if (row.code.isEmpty()) {
        errors.append(makeError(lineNumber, "⚠️ Hiányzó barcode (code)", row));
    }

    if (row.entityType.isEmpty()) {
        errors.append(makeError(lineNumber, "⚠️ Hiányzó entityType", row));
    }

    // Dátumok: introduced kötelező, retired opcionális
    {
        const auto introduced = QDateTime::fromString(row.introducedAtStr, Qt::ISODate);
        if (!introduced.isValid()) {
            errors.append(makeError(lineNumber, "⚠️ Érvénytelen introducedAt (ISO)", row));
        }
        if (!row.retiredAtStr.isEmpty()) {
            const auto retired = QDateTime::fromString(row.retiredAtStr, Qt::ISODate);
            if (!retired.isValid()) {
                errors.append(makeError(lineNumber, "⚠️ Érvénytelen retiredAt (ISO)", row));
            }
        }
    }

    return errors;
}


// --- Stage 2: Build ---
std::optional<BarcodeRecord>
BarcodeRepository::buildRecordFromRow(const BarcodeRow& row,
                                      CsvImporter::FileContext& ctx)
{
    auto rowErrors = validateBarcodeRow(row, ctx.currentLineNumber());
    ctx.addErrors(rowErrors);

    if (!rowErrors.isEmpty())
        return std::nullopt;

    BarcodeRecord rec;
    rec.code         = row.code;
    rec.entityType   = row.entityType;
    rec.introducedAt = QDateTime::fromString(row.introducedAtStr, Qt::ISODate);

    if (!row.retiredAtStr.isEmpty()) {
        const auto retired = QDateTime::fromString(row.retiredAtStr, Qt::ISODate);
        if (retired.isValid()) rec.retiredAt = retired;
    }

    // Status getterből számolódik → nincs explicit mező
    return rec;
}


// --- Stage 3: Load & Assemble ---
QVector<CsvImporter::AuditedRow<BarcodeRepository::BarcodeRow>>
BarcodeRepository::loadBarcodeRows(CsvImporter::FileContext& ctx)
{
    // A CsvImporter ugyanazt a konverziós keretrendszert használja, mint a MaterialRepository
    return CsvImporter::readAndConvert<CsvImporter::AuditedRow<BarcodeRepository::BarcodeRow>>(ctx, convertRowToBarcodeRow);
}

// --- Entry Point ---
bool BarcodeRepository::loadFromCSV(BarcodeRegistry& registry)
{
    auto& helper = FileNameHelper::instance();
    if (!helper.isInitialized()) {
        zWarning("❌ A FileNameHelper nincs inicializálva.");
        return false;
    }

    // Javaslat: implementáld a FileNameHelper-ben:
    //   QString getBarcodeCsvFile() const;
    const QString fn = helper.getBarcodeCsvFile(); // auditbarát: központi helyről jön
    CsvImporter::FileContext ctx("Barcode import", fn);

    // 1) Beolvasás + konvertálás
    const QVector<CsvImporter::AuditedRow<BarcodeRow>> rows = loadBarcodeRows(ctx);

    // 2) Build – típusos rekordok
    const QVector<BarcodeRecord> records =
        CsvImporter::buildAll<BarcodeRow, BarcodeRecord>(rows, buildRecordFromRow, ctx);

    // 3) Duplikáció kockázat – globális uniqueness a CSV-ben is
    //    Hunglish: fail-fast, ha a CSV-ben ugyanaz a code többször szerepel.
    {
        QSet<QString> seen;
        for (const auto& r : records) {
            if (seen.contains(r.code)) {
                ctx.addError(ctx.currentLineNumber(),
                             QString("⚠️ Duplikált barcode a CSV-ben: %1").arg(r.code));
            } else {
                seen.insert(r.code);
            }
        }
    }

    if (ctx.hasErrors()) {
        zWarning(QString("⚠️ Hibák a barcode import során (%1)").arg(ctx.errorsSize()));
    }

    // 4) Assemble – a registrybe töltés (globális uniqueness itt is érvényesül)
    int accepted = 0;
    for (const auto& rec : records) {
        if (registry.isBarcodeUnique(rec.code)) {
            // Régisztáció a registrybe – a registry a MaterialRegistry mintájára auditál
            if (registry.registerData_volatile(rec)) {
                ++accepted;
            }
        } else {
            // CSV-ben validnak tűnt, de a registryben már szerepel – audit WARN
            zWarning(QString("❌ Barcode duplikált a registryben is: %1").arg(rec.code));
        }
    }

    zInfo(QString("📊 BarcodeRepository: %1 rekord beolvasva, %2 regisztrálva")
              .arg(records.size())
              .arg(accepted));

    return !records.isEmpty();
}

// --- Export (opcionális) ---
bool BarcodeRepository::saveToCSV(const BarcodeRegistry& registry, const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        zWarning(QString("❌ Nem sikerült megnyitni a fájlt írásra: %1").arg(path));
        return false;
    }

    QTextStream ts(&f);
    ts.setEncoding(QStringConverter::Utf8);

    // Új header – letisztított szerződés
    ts << "barCode;entityType;introducedAt;retiredAt\n";

    const auto& all = registry.readAll();
    for (const auto& r : all) {
        ts << r.code << ";"
           << r.entityType << ";"
           << r.introducedAt.toString(Qt::ISODate) << ";"
           << (r.retiredAt.has_value() ? r.retiredAt->toString(Qt::ISODate) : "")
           << "\n";
    }

    zInfo(QString("📦 BarcodeRepository: CSV export kész → %1").arg(path));
    return true;
}
