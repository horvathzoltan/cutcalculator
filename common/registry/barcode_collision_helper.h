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
 *   - megnézi a globális uniqueness-et (BarcodeRegistry)
 *   - előkeresi az ütköző entitást (RegistryManager)
 *   - összerak egy emberbarát üzenetet (typeName, name, barcode, status)
 */
class BarcodeCollisionHelper {
public:
    struct RowInfo {
        QString barcode;
        QString name;
    };

    static std::optional<CsvImporter::RowError>
    makeBarcodeCollisionError(const QString& myTypeName,
                              const RowInfo& row,
                              int lineNumber);

    static QString formatRowDisplay(const RowInfo& row);
    static QString formatStatus(const BarcodeRecord& rec);
};
