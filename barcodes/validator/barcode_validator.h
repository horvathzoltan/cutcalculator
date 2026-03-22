#pragma once

#include <QString>
#include <QUuid>
#include "barcodes/registry/barcode_registry.h"
#include "common/csv/filecontext.h"

/**
 * @brief BarcodeValidator – vékony, központi validator helper.
 *
 * - Legyen egy egyértelmű kapu a registryben, amin minden elem átmegy.
 * - Nem keverjük a business és az audit logikát — ő csak check és report.
 */
class BarcodeValidator {
public:
    /**
     * @brief checkAndRegister – ellenőrzi a globális uniqueness-t, és ha oké,
     * regisztrálja az új barcode-ot. Ha nem oké, audit hibát tesz a ctx-be.
     *
     * @return true, ha sikerült regisztrálni; false, ha ütközött vagy hibás
     */
    static bool checkAndRegister(const QString& code,
                                 const QString& entityType,
                                 const QUuid& id,
                                 const QString& name,
                                 CsvImporter::FileContext& ctx);

    static bool checkAndRegisterUI(const QString& code,
                                   const QString& entityType,
                                   const QUuid& id,
                                   const QString& name,
                                   QString& outErrorMessage);

    /**
     * @brief retire – nyugdíjazza a barcode-ot (audit + státusz),
     * nem dob hibát, ha a kód nem létezik, csak figyelmeztet.
     */
    static void retire(const QString& code, const QString& reason);
};
