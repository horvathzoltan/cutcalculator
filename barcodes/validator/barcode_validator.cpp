#include "barcodes/validator//barcode_validator.h"
#include "common/logger/event_logger.h"

#include <barcodes/helpers/barcode_collision_helper.h>


#include "barcodes/validator/barcode_validator.h"
#include "common/logger/event_logger.h"
#include <barcodes/helpers/barcode_collision_helper.h>

#include <QRegularExpression>

// =========================
// PRIVATE COMMON IMPLEMENTATION
// =========================

bool BarcodeValidator::checkAndRegister_impl(const QString& code,
                                             const QString& entityType,
                                             const QUuid& id,
                                             const QString& name,
                                             Mode mode,
                                             CsvImporter::FileContext* ctx,
                                             QString* outErrorMessage)
{
    auto& br = BarcodeRegistry::instance();

    // A felhasználó által megadott kód whitespace-mentesítése
    const QString trimmedCode = code.trimmed();

    // 1) Üres kód tiltása
    if (trimmedCode.isEmpty()) {
        const QString msg = "Barcode cannot be empty";

        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }

        zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
        return false;
    }

    // 1/b) Normalize → ékezetek eltávolítása, nagybetűsítés
    QString normalized = trimmedCode.normalized(QString::NormalizationForm_D);
    normalized.remove(QRegularExpression("\\p{Mn}")); // diakritikus jelek eltávolítása
    normalized = normalized.toUpper();

    // Ha a normalize után megváltozott a kód → tiltott karakter volt (ékezet, unicode stb.)
    if (normalized != trimmedCode.toUpper()) {
        const QString msg = QString("Invalid characters in barcode: %1").arg(code);

        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }

        zEventERROR(msg);
        return false;
    }

    // 1/c) Regex alapú karakterkészlet ellenőrzés (A–Z, 0–9, '-')
    static QRegularExpression allowed("^[A-Z0-9-]+$");

    if (!allowed.match(normalized).hasMatch()) {
        const QString msg = QString("Invalid characters in barcode: %1").arg(code);

        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }

        zEventERROR(msg);
        return false;
    }

    // 2) Collision check – normalized kóddal
    if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
            entityType,
            BarcodeCollisionHelper::RowInfo{ normalized, name, id },
            (mode == Mode::CSV ? ctx->currentLineNumber() : 0)))
    {
        if (mode == Mode::CSV) {
            ctx->addError(err->lineIndex(), err->errorMessage(),
                          err->barcode(), err->name());
        } else {
            *outErrorMessage = err->errorMessage();
        }

        zEventERROR(err->errorMessage());
        return false;
    }

    // 3) Ledger regisztráció – normalized kóddal
    if (!br.registerNew(normalized, entityType, id, name)) {

        const QString msg = QString("Barcode registration failed: %1")
        .arg(normalized);

        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }

        zEventERROR(msg);
        return false;
    }

    // 4) Sikeres regisztráció
    if (mode == Mode::UI) {
        zEventINFO(QString("Barcode registered (UI): %1 → %2")
                       .arg(name, normalized));
    }

    return true;
}


// =========================
// PUBLIC CSV WRAPPER
// =========================

bool BarcodeValidator::checkAndRegister_CSV(const QString& code,
                                            const QString& entityType,
                                            const QUuid& id,
                                            const QString& name,
                                            CsvImporter::FileContext& ctx)
{
    return checkAndRegister_impl(code, entityType, id, name,
                                 Mode::CSV, &ctx, nullptr);
}

// =========================
// PUBLIC UI WRAPPER
// =========================

bool BarcodeValidator::checkAndRegister_UI(const QString& code,
                                           const QString& entityType,
                                           const QUuid& id,
                                           const QString& name,
                                           QString& outErrorMessage)
{
    return checkAndRegister_impl(code, entityType, id, name,
                                 Mode::UI, nullptr, &outErrorMessage);
}


// bool BarcodeValidator::checkAndRegister_CSV(const QString& code,
//                                         const QString& entityType,
//                                         const QUuid& id,
//                                         const QString& name,
//                                         CsvImporter::FileContext& ctx)
// {
//     auto& br = BarcodeRegistry::instance();
//     const QString trimmedCode = code.trimmed();

//     // 1) Üres kód – korai, egyértelmű hiba
//     if (trimmedCode.isEmpty()) {
//         ctx.addError(ctx.currentLineNumber(),
//                      QString("Barcode cannot be empty"),
//                      code,
//                      name);
//         zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
//         return false;
//     }

//     // 2) Globális uniqueness + domain collision check
//     if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
//             entityType,
//             BarcodeCollisionHelper::RowInfo{ trimmedCode, name, id },
//             ctx.currentLineNumber()))
//     {
//         ctx.addError(err->lineIndex(), err->errorMessage(), err->barcode(), err->name());
//         return false;
//     }

//     // 3) Regisztráció a registryben
//     if (!br.registerNew(trimmedCode, entityType, id, name)) {
//         // Elvileg ide nem kéne eljutni – ha mégis, auditáljuk
//         ctx.addError(ctx.currentLineNumber(),
//                      QString("Barcode registration failed: %1").arg(trimmedCode),
//                      trimmedCode,
//                      name);

//         return false;
//     }

//     return true;
// }

// bool BarcodeValidator::checkAndRegister_UI(const QString& code,
//                                           const QString& entityType,
//                                           const QUuid& id,
//                                           const QString& name,
//                                           QString& outErrorMessage)
// {
//     auto& br = BarcodeRegistry::instance();
//     const QString trimmedCode = code.trimmed();

//     if (trimmedCode.isEmpty()) {
//         outErrorMessage = "Barcode cannot be empty";
//         zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
//         return false;
//     }

//     if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
//             entityType,
//             BarcodeCollisionHelper::RowInfo{ trimmedCode, name, id },
//             0))
//     {
//         outErrorMessage = err->errorMessage();
//         zEventERROR(err->errorMessage());
//         return false;
//     }

//     if (!br.registerNew(trimmedCode, entityType, id, name)) {
//         outErrorMessage = QString("Failed to register barcode: %1").arg(trimmedCode);
//         zEventERROR(outErrorMessage);
//         return false;
//     }

//     zEventINFO(QString("Barcode registered (UI): %1 → %2").arg(name, trimmedCode));
//     return true;
// }



