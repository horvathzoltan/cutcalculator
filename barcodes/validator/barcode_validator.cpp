#include "barcodes/validator/barcode_validator.h"
#include "common/logger/event_logger.h"
#include <barcodes/helpers/barcode_collision_helper.h>
#include <QRegularExpression>


// =========================
// PRIVATE COMMON IMPLEMENTATION
// =========================


bool BarcodeValidator::validate_impl(const QString& code,
                                     const QString& entityType,
                                     const QUuid& id,
                                     const QString& name,
                                     Mode mode,
                                     CsvImporter::FileContext* ctx,
                                     QString* outErrorMessage,
                                     QString& outNormalized)
{
    Q_UNUSED(id)

    const QString trimmed = code.trimmed();
    if (trimmed.isEmpty()) {
        const QString msg = "Barcode cannot be empty";
        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }
        return false;
    }

    QString normalized = trimmed.normalized(QString::NormalizationForm_D);
    normalized.remove(QRegularExpression("\\p{Mn}"));
    normalized = normalized.toUpper();

    if (normalized != trimmed.toUpper()) {
        const QString msg = QString("Invalid characters in barcode: %1").arg(code);
        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }
        return false;
    }

    static QRegularExpression allowed("^[A-Z0-9-]+$");
    if (!allowed.match(normalized).hasMatch()) {
        const QString msg = QString("Invalid characters in barcode: %1").arg(code);
        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, code, name);
        } else {
            *outErrorMessage = msg;
        }
        return false;
    }

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
        return false;
    }

    outNormalized = normalized;
    return true;
}

bool BarcodeValidator::register_impl(const QString& normalizedCode,
                                     const QString& entityType,
                                     const QUuid& id,
                                     const QString& name,
                                     Mode mode,
                                     CsvImporter::FileContext* ctx,
                                     QString* outErrorMessage)
{
    auto& br = BarcodeRegistry::instance();

    if (!br.registerNew(normalizedCode, entityType, id, name)) {
        const QString msg = QString("Barcode registration failed: %1")
        .arg(normalizedCode);

        if (mode == Mode::CSV) {
            ctx->addError(ctx->currentLineNumber(), msg, normalizedCode, name);
        } else {
            *outErrorMessage = msg;
        }
        return false;
    }

    return true;
}

// bool BarcodeValidator::checkAndRegister_impl(const QString& code,
//                                              const QString& entityType,
//                                              const QUuid& id,
//                                              const QString& name,
//                                              Mode mode,
//                                              CsvImporter::FileContext* ctx,
//                                              QString* outErrorMessage)
// {
//     auto& br = BarcodeRegistry::instance();

//     // A felhasználó által megadott kód whitespace-mentesítése
//     const QString trimmedCode = code.trimmed();

//     // 1) Üres kód tiltása
//     if (trimmedCode.isEmpty()) {
//         const QString msg = "Barcode cannot be empty";

//         if (mode == Mode::CSV) {
//             ctx->addError(ctx->currentLineNumber(), msg, code, name);
//         } else {
//             *outErrorMessage = msg;
//         }

//         zEventERROR(QString("Empty barcode for %1 (%2)").arg(name, entityType));
//         return false;
//     }

//     // 1/b) Normalize → ékezetek eltávolítása, nagybetűsítés
//     QString normalized = trimmedCode.normalized(QString::NormalizationForm_D);
//     normalized.remove(QRegularExpression("\\p{Mn}")); // diakritikus jelek eltávolítása
//     normalized = normalized.toUpper();

//     // Ha a normalize után megváltozott a kód → tiltott karakter volt (ékezet, unicode stb.)
//     if (normalized != trimmedCode.toUpper()) {
//         const QString msg = QString("Invalid characters in barcode: %1").arg(code);

//         if (mode == Mode::CSV) {
//             ctx->addError(ctx->currentLineNumber(), msg, code, name);
//         } else {
//             *outErrorMessage = msg;
//         }

//         zEventERROR(msg);
//         return false;
//     }

//     // 1/c) Regex alapú karakterkészlet ellenőrzés (A–Z, 0–9, '-')
//     static QRegularExpression allowed("^[A-Z0-9-]+$");

//     if (!allowed.match(normalized).hasMatch()) {
//         const QString msg = QString("Invalid characters in barcode: %1").arg(code);

//         if (mode == Mode::CSV) {
//             ctx->addError(ctx->currentLineNumber(), msg, code, name);
//         } else {
//             *outErrorMessage = msg;
//         }

//         zEventERROR(msg);
//         return false;
//     }

//     // 2) Collision check – normalized kóddal
//     if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
//             entityType,
//             BarcodeCollisionHelper::RowInfo{ normalized, name, id },
//             (mode == Mode::CSV ? ctx->currentLineNumber() : 0)))
//     {
//         if (mode == Mode::CSV) {
//             ctx->addError(err->lineIndex(), err->errorMessage(),
//                           err->barcode(), err->name());
//         } else {
//             *outErrorMessage = err->errorMessage();
//         }

//         zEventERROR(err->errorMessage());
//         return false;
//     }

//     // 3) Ledger regisztráció – normalized kóddal
//     if (!br.registerNew(normalized, entityType, id, name)) {

//         const QString msg = QString("Barcode registration failed: %1")
//         .arg(normalized);

//         if (mode == Mode::CSV) {
//             ctx->addError(ctx->currentLineNumber(), msg, code, name);
//         } else {
//             *outErrorMessage = msg;
//         }

//         zEventERROR(msg);
//         return false;
//     }

//     // 4) Sikeres regisztráció
//     if (mode == Mode::UI) {
//         zEventINFO(QString("Barcode registered (UI): %1 → %2")
//                        .arg(name, normalized));
//     }

//     return true;
// }

bool BarcodeValidator::checkAndRegister_impl(const QString& code,
                                             const QString& entityType,
                                             const QUuid& id,
                                             const QString& name,
                                             Mode mode,
                                             CsvImporter::FileContext* ctx,
                                             QString* outErrorMessage)
{
    QString normalized;

    if (!validate_impl(code, entityType, id, name,
                       mode, ctx, outErrorMessage, normalized))
        return false;

    if (!register_impl(normalized, entityType, id, name,
                       mode, ctx, outErrorMessage))
        return false;

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

bool BarcodeValidator::validate_UI(const QString& code,
                                   const QString& entityType,
                                   const QUuid& id,
                                   const QString& name,
                                   QString& outErrorMessage)
{
    QString normalized;
    return validate_impl(code, entityType, id, name,
                         Mode::UI, nullptr, &outErrorMessage, normalized);
}

bool BarcodeValidator::checkAndRegister_UI(const QString& code,
                                           const QString& entityType,
                                           const QUuid& id,
                                           const QString& name,
                                           QString& outErrorMessage)
{
    QString normalized;

    if (!validate_impl(code, entityType, id, name,
                       Mode::UI, nullptr, &outErrorMessage, normalized))
        return false;

    if (!register_impl(normalized, entityType, id, name,
                       Mode::UI, nullptr, &outErrorMessage))
        return false;

    BarcodeRegistry::instance().persistIfDirty();
    return true;
}

