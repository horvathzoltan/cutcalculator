#pragma once

#include <optional>
#include <QString>

#include "common/csv/rowerror.h"
#include "common/registry/barcode_table.h"

//#include "common/csv/filecontext.h"

/**
 * @brief BarcodeCollisionHelper – egységes, auditbarát ütközés riportoló.
 *
 * Hunglish: ne legyen copy-paste a Material/Product validátorban,
 * hanem hívd ezt a helpert, és ő megoldja:
 *  - megnézi a globális uniqueness-et (BarcodeTable)
 *  - előkeresi az ütköző entitást (RegistryManager)
 *  - összerak egy emberbarát üzenetet (typeName, name, barcode, status)
 *
 * Használat:
 *  - add át a saját típusnevedet (pl. "Material" vagy "Product")
 *  - a sorból a barcode+name értékeket
 *  - sor számot (lineNumber), hogy CSV-ben checklist-barát legyen
 *
 * Integráció:
 *  - MaterialRepository::validateMaterialRow → hívás makeBarcodeCollisionError(...)
 *  - ProductRepository::validateProductRow → hívás makeBarcodeCollisionError(...)
 */
class BarcodeCollisionHelper {
public:
    // Hunglish: a RowInfo könnyű adapter, hogy ne kelljen ROW típusra konvertálni.
    struct RowInfo {
        QString barcode;
        QString name;
    };

    /**
     * @brief makeBarcodeCollisionError – ha a barcode nem egyedi, visszaad egy sorhibát.
     *
     * @param myTypeName  Emberbarát típusnév (pl. "Material", "Product")
     * @param row         Minimál adapter (barcode + name)
     * @param lineNumber  CSV sor sorszám
     *
     * @return std::optional<CsvImporter::RowError>
     *         - ütközés esetén RowError, különben std::nullopt
     */
    static std::optional<CsvImporter::RowError>
    makeBarcodeCollisionError(const QString& myTypeName,
                              const RowInfo& row,
                              int lineNumber);

    /**
     * @brief formatRowDisplay – emberbarát megjelenítés (név + [barcode]).
     * Hunglish: ez stabil, checklist-barát, és minden riportban ugyanaz.
     */
    static QString formatRowDisplay(const RowInfo& row);

    /**
     * @brief formatStatus – BarcodeTable státusz stringné alakítása.
     */
    static QString formatStatus(BarcodeTable::Status st);
};

