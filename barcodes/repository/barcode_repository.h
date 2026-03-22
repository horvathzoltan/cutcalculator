#pragma once

#include <QString>
#include <QVector>
#include "common/csv/csvimporter.h"
#include "barcodes/model/barcode_record.h"
#include "barcodes/registry/barcode_registry.h"

/**
 * BarcodeRepository
 *
 * Háromfázisú CSV import minta (MaterialRepository mintájára):
 *  - Stage 1: Convert → CSV sor → BarcodeRow
 *  - Stage 2.5: Validate → BarcodeRow → hibák (auditbarát visszajelzés)
 *  - Stage 2: Build → BarcodeRow → BarcodeRecord
 *  - Stage 3: Assemble → BarcodeRecord → BarcodeRegistry
 *
 * *
 * - A CSV minden sorában egy ledger rekord van: barCode;entityType;introducedAt;retiredAt
 * - Dátumok ISO 8601 formátumban (Qt::ISODate), retiredAt üres, ha nincs.
 * - A globális uniqueness elv érvényes: ugyanaz a "barCode" csak egyszer szerepelhet a teljes CSV-ben.
 */
class BarcodeRepository {
public:
    /**
     * Entry point – CSV betöltés és registry feltöltés.
     * - A FileNameHelper-ből várja a CSV útvonalát (pl. pathFor()).
     * - Audit hibák esetén warning és részösszegző log.
     */
//    static bool loadFromCSV(BarcodeRegistry& registry);
    static bool load(QVector<BarcodeRecord>& out);

    /**
     * Opcionális export – a registry jelenlegi tartalmát kiírja CSV-be.
     * - Auditbarát header sorral, ISO dátumokkal, pontos statusz értékkel.
     */
    static bool saveToCSV(const BarcodeRegistry& registry);

private:
    // 📥 CSV sor logikai modellje (raw → typed, de még nem BarcodeRecord)
    struct BarcodeRow {
        QString code;
        QString entityType;
        QString introducedAtStr; // ISO 8601
        QString retiredAtStr;    // ISO 8601 vagy üres
    };

    // --- Stage 1: Convert ---
    static std::optional<CsvImporter::AuditedRow<BarcodeRow>>
    convertRowToBarcodeRow(const QVector<QString>& parts, CsvImporter::FileContext& ctx);

    // --- Stage 2.5: Validate ---
    static QVector<CsvImporter::RowError>
    validateBarcodeRow(const BarcodeRow& row, int lineNumber);

    // --- Stage 2: Build ---
    static std::optional<BarcodeRecord>
    buildRecordFromRow(const BarcodeRow& row, CsvImporter::FileContext& ctx);

    // --- Stage 3: Load & Assemble ---
    static QVector<CsvImporter::AuditedRow<BarcodeRow>>
    loadBarcodeRows(CsvImporter::FileContext& ctx);

    static CsvImporter::RowError
    makeError(int lineNumber, const QString& message, const BarcodeRow& row);
};
