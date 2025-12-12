#include "barcode_validator.h"
#include "common/logger/event_logger.h"

bool BarcodeValidator::checkAndRegister(const QString& code,
                                        const QString& entityType,
                                        const QUuid& id,
                                        const QString& name,
                                        CsvImporter::FileContext& ctx)
{
    // Quick sanity: üres kód nem oké (Hunglish: ezt jobb korán elkapni)
    if (code.trimmed().isEmpty()) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("Empty barcode is not allowed"),
                     code,
                     name);
        zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
        return false;
    }

    if (!BarcodeTable::instance().checkUnique(code, entityType, id)) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("Barcode collision: %1").arg(code),
                     code,
                     name);
        zEventERROR(QString("Barcode collision: %1 (%2)").arg(code, entityType));
        return false;
    }

    // Ha unique, megpróbáljuk bejegyezni
    if (!BarcodeTable::instance().registerNew(code, entityType, id)) {
        // Elvileg ide nem kéne eljutni, de ha race vagy más anomália, legyen audit
        ctx.addError(ctx.currentLineNumber(),
                     QString("Failed to register barcode: %1").arg(code),
                     code,
                     name);
        zEventERROR(QString("Failed to register barcode: %1 (%2)").arg(code, entityType));
        return false;
    }

    return true;
}

void BarcodeValidator::retire(const QString& code, const QString& reason) {
    BarcodeTable::instance().retire(code, reason);
}
