#pragma once

#include <optional>
#include <QString>
#include "common/csv/rowerror.h"
#include "barcodes/registry/barcode_registry.h"

/**
 * @brief BarcodeCollisionHelper – egységes, auditbarát ütközés riportoló.
 *
 * - Ne legyen copy-paste a Material/Product validátorban,
 *   hanem hívd ezt a helpert, és ő megoldja:
 *   - ledger-alapú globális uniqueness ellenőrzés
 *   - ütköző entitás lookup (RegistryManager)
 *   - egységes, auditbarát üzenet (typeName, name, barcode, status)
 */
class BarcodeCollisionHelper {
public:
    struct RowInfo {
        QString barcode;
        QString name;
        std::optional<QUuid> entityId;   // 🔥 ÚJ: az entitás azonosítója
    };

    static std::optional<CsvImporter::RowError>
    makeBarcodeCollisionError(const QString& myTypeName,
                              const RowInfo& row,
                              int lineNumber);

    static QString formatRowDisplay(const RowInfo& row);
    static QString formatStatus(const BarcodeRecord& rec);
};
