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
    static bool checkAndRegister_CSV(const QString& code,
                                     const QString& entityType,
                                     const QUuid& id,
                                     const QString& name,
                                     CsvImporter::FileContext& ctx);

    static bool checkAndRegister_UI(const QString& code,
                                    const QString& entityType,
                                    const QUuid& id,
                                    const QString& name,
                                    QString& outErrorMessage);

private:
    enum class Mode { CSV, UI };

    static bool checkAndRegister_impl(const QString& code,
                                      const QString& entityType,
                                      const QUuid& id,
                                      const QString& name,
                                      Mode mode,
                                      CsvImporter::FileContext* ctx,
                                      QString* outErrorMessage);
};

