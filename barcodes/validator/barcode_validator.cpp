#include "barcodes/validator//barcode_validator.h"
#include "common/logger/event_logger.h"

#include <barcodes/helpers/barcode_collision_helper.h>

bool BarcodeValidator::checkAndRegister(const QString& code,
                                        const QString& entityType,
                                        const QUuid& id,
                                        const QString& name,
                                        CsvImporter::FileContext& ctx)
{
    auto& br = BarcodeRegistry::instance();
    const QString trimmedCode = code.trimmed();

    // 1) Üres kód – korai, egyértelmű hiba
    if (trimmedCode.isEmpty()) {
        ctx.addError(ctx.currentLineNumber(),
                     QString("Barcode cannot be empty"),
                     code,
                     name);
        zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
        return false;
    }

    // 2) Globális uniqueness + domain collision check
    if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
            entityType,
            BarcodeCollisionHelper::RowInfo{ trimmedCode, name, id },
            ctx.currentLineNumber()))
    {
        ctx.addError(err->lineIndex(), err->errorMessage(), err->barcode(), err->name());
        return false;
    }

    // 3) Regisztráció a registryben
    if (!br.registerNew(trimmedCode, entityType, id, name)) {
        // Elvileg ide nem kéne eljutni – ha mégis, auditáljuk
        ctx.addError(ctx.currentLineNumber(),
                     QString("Barcode registration failed: %1").arg(trimmedCode),
                     trimmedCode,
                     name);

        return false;
    }

    return true;
}

bool BarcodeValidator::checkAndRegisterUI(const QString& code,
                                          const QString& entityType,
                                          const QUuid& id,
                                          const QString& name,
                                          QString& outErrorMessage)
{
    auto& br = BarcodeRegistry::instance();
    const QString trimmedCode = code.trimmed();

    if (trimmedCode.isEmpty()) {
        outErrorMessage = "Barcode cannot be empty";
        zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
        return false;
    }

    if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
            entityType,
            BarcodeCollisionHelper::RowInfo{ trimmedCode, name, id },
            0))
    {
        outErrorMessage = err->errorMessage();
        zEventERROR(err->errorMessage());
        return false;
    }

    if (!br.registerNew(trimmedCode, entityType, id, name)) {
        outErrorMessage = QString("Failed to register barcode: %1").arg(trimmedCode);
        zEventERROR(outErrorMessage);
        return false;
    }

    zEventINFO(QString("Barcode registered (UI): %1 → %2").arg(name, trimmedCode));
    return true;
}


void BarcodeValidator::retire(const QString& code, const QString& reason) {
    auto& br = BarcodeRegistry::instance();
    const QString trimmedCode = code.trimmed();

    if (!br.retire(trimmedCode, reason)) {
        zEventWARN(QString("Barcode retire ignored: %1").arg(trimmedCode));
    }
}

