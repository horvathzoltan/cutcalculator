#include "barcodes/registry/barcode_registry.h"
#include "common/logger/logger.h"
#include "common/logger/event_logger.h"
#include "common/system/verbose_manager.h"
#include "barcodes/repository/barcode_repository.h"
#include "common/utils/filename_helper.h"
#include "barcodes/helpers/barcode_collision_helper.h"

// --- Lookup API ---

const BarcodeRecord*
BarcodeRegistry::findByCode(const QString& code) const
{
    return findIf([&](const BarcodeRecord& r){
        return r.code == code;
    });
}

bool BarcodeRegistry::isBarcodeUnique(const QString& code) const
{
    return !existsBy([&](const BarcodeRecord& r){
        return r.code == code;
    });
}

// --- Workflow hookok ---

bool BarcodeRegistry::validateDomain(const BarcodeRecord& r) const
{
    if (r.code.trimmed().isEmpty())
        return false;
    if (r.entityType.trimmed().isEmpty())
        return false;
    if (!r.introducedAt.isValid())
        return false;
    return true;
}

bool BarcodeRegistry::validateDuplicate(const BarcodeRecord& r) const
{
    // Globális uniqueness: ugyanazzal a kóddal nem lehet másik rekord
    return !existsBy([&](const BarcodeRecord& x){
        return x.code == r.code && x.entityType == r.entityType
               && x.introducedAt != r.introducedAt;
    });
}

bool BarcodeRegistry::beforeInsert(const BarcodeRecord& r)
{
    Q_UNUSED(r);
    // Jelenleg nincs extra logika – a caller (registerNew) tölti az introducedAt-et.
    return true;
}

bool BarcodeRegistry::beforeUpdate(const BarcodeRecord& r)
{
    Q_UNUSED(r);
    // Itt is maradhatunk passzívak – a retire() állítja a retiredAt-et.
    return true;
}

// --- Log hookok ---

void BarcodeRegistry::onInsertLog(const BarcodeRecord& r)
{
    if (IS_VERBOSE_THIS()) {
        zInfo(QString("Barcode registered: %1 [%2]")
                  .arg(r.entityType, r.code));
    }
}

void BarcodeRegistry::onUpdateLog(const BarcodeRecord& r)
{
    if (IS_VERBOSE_THIS()) {
        zInfo(QString("Barcode updated: %1 [%2], status=%3")
                  .arg(r.entityType,
                       r.code,
                       r.isActive() ? "Active" : "Retired"));
    }
}

void BarcodeRegistry::onRemoveLog(const BarcodeRecord& r)
{
    zInfo(QString("Barcode removed from registry: %1 [%2]")
              .arg(r.entityType, r.code));
}

// --- Public API: registerNew / retire ---

bool BarcodeRegistry::registerNew(const QString& code,
                                  const QString& entityType,
                                  const QUuid& id,
                                  const QString& name)
{
    const QString trimmedCode = code.trimmed();
    if (trimmedCode.isEmpty())
        return false;

    if (auto* rec = const_cast<BarcodeRecord*>(findByCode(trimmedCode))) {
        // Már létezik ez a kód
        if (!rec->entityId.has_value()) {
            // Csak entityId hiányzik → frissítsük
            BarcodeRecord updated = *rec;
            updated.entityId = id;

            if (!update(updated)) {
                zWarning(QString("⚠️ Failed to update existing barcode: %1").arg(trimmedCode));
                return false;
            }

            persist();
            return true;
        } else {
            // EntityId már van, és ha más, akkor ütközés
            if (rec->entityId != id) {


                // if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
                //         entityType,
                //         BarcodeCollisionHelper::RowInfo{ trimmedCode, name },
                //         /* lineNumber */ 0))


                 if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
                       entityType,
                       BarcodeCollisionHelper::RowInfo{ trimmedCode, name, id },
                       /* lineNumber */ 0))

                {
                    zWarning(err->errorMessage());
                    zEventWARN(err->errorMessage());
                }
                return false;
            }
            // Ugyanaz az entitás → nincs teendő
            return true;
        }
    }

    // Új kód → létrehozás
    BarcodeRecord rec;
    rec.id = QUuid::createUuid(); // 🔑 belső ID
    rec.code         = trimmedCode;
    rec.entityType   = entityType;
    rec.entityId     = id;
    rec.introducedAt = QDateTime::currentDateTime();

    if (!insert(rec)) {
        zWarning(QString("⚠️ Failed to insert new barcode: %1").arg(trimmedCode));
        return false;
    }

    persist();
    return true;
}

bool BarcodeRegistry::retire(const QString& code, const QString& reason)
{
    const QString trimmedCode = code.trimmed();
    if (trimmedCode.isEmpty())
        return false;

    if (const BarcodeRecord* rec = findByCode(trimmedCode)) {
        if (rec->retiredAt.has_value()) {
            zInfo(QString("Retire requested for already retired code: %1").arg(trimmedCode));
            return false;
        }

        BarcodeRecord updated = *rec;
        updated.retiredAt = QDateTime::currentDateTime();

        if (!update(updated)) {
            zWarning(QString("⚠️ Failed to retire barcode: %1").arg(trimmedCode));
            return false;
        }

        zInfo(QString("Barcode retired: %1 (reason=%2)").arg(trimmedCode, reason));
        persist();
        return true;
    }

    zWarning(QString("Retire requested for unknown code: %1").arg(trimmedCode));
    return false;
}

// --- Persist ---

void BarcodeRegistry::persist() const
{
    const QString path = FileNameHelper::instance().getBarcodeCsvFile();
    if (path.isEmpty())
        return;

    // Egyelőre megtartjuk a meglévő API-t
    BarcodeRepository::saveToCSV(*this, path);
}

// #include "barcodes/registry/barcode_registry.h"
// #include "common/logger/logger.h"
// #include "common/logger/event_logger.h"
// #include "common/system/verbose_manager.h"
// #include "barcodes/repository/barcode_repository.h"
// #include "common/utils/filename_helper.h"
// #include "common/utils/optional_utils.h"
// #include "barcodes/helpers/barcode_collision_helper.h"

// BarcodeRegistry& BarcodeRegistry::instance() {
//     static BarcodeRegistry reg;
//     return reg;
// }

// const BarcodeRecord* BarcodeRegistry::findByCode(const QString& code) const {
//     for (const auto& r : _items) {
//         if (r.code == code)
//             return &r;
//     }
//     return nullptr;
// }

// bool BarcodeRegistry::isBarcodeUnique(const QString& code) const {
//     for (const auto& r : _items) {
//         if (r.code == code)
//             return false;
//     }
//     return true;
// }

// // const BarcodeIdentifiableEntity* BarcodeRegistry::findEntityById(const QUuid& id) const {
// //     // for (const auto& r : _items) {
// //     //     if (r.entityId.has_value() && r.entityId == id)
// //     //         return reinterpret_cast<const BarcodeIdentifiableEntity*>(&r);
// //     // }
// //     return nullptr;
// // }


// bool BarcodeRegistry::add(const BarcodeRecord& r) {
//     if (!isBarcodeUnique(r.code)) {
//         zWarning(QString("Barcode collision: %1 (%2)")
//                      .arg(r.code, r.entityType));
//         zEventWARN(QString("Barcode ütközés: %1 (%2)")
//                        .arg(r.code, r.entityType));
//         return false;
//     }
//     _items.append(r);
//     return true;
// }

// bool BarcodeRegistry::registerNew(const QString& code,
//                                   const QString& entityType,
//                                   const QUuid& id,
//                                   const QString& name)
// {
//     if (auto* rec = const_cast<BarcodeRecord*>(findByCode(code))) {
//         // Már létezik → hazatérés
//         if (!rec->entityId.has_value()) {
//             rec->entityId = id; // frissítjük az entityId-t
//             return true;
//         } else {
//             // Ha már van entityId és más → ütközés
//             if (rec->entityId != id) {
//                 if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
//                         entityType,
//                         BarcodeCollisionHelper::RowInfo{ code, name },
//                         /* lineNumber */ 0)) // ha nincs CSV sor, adhatsz 0-t vagy -1-et
//                 {
//                     // audit log – ugyanazt az üzenetet használjuk
//                     zWarning(err->errorMessage());
//                     zEventWARN(err->errorMessage());
//                 }
//                 return false;
//             }
//             return true; // ugyanaz az entitás → oké
//         }
//     }

//     // Új kód → rendes regisztráció
//     BarcodeRecord rec;
//     rec.code         = code;
//     rec.entityType   = entityType;
//     rec.entityId     = id;
//     rec.introducedAt = QDateTime::currentDateTime();
//     if (!add(rec)) return false;

//     persist();
//     return true;
// }


// // // 🔐 Privát belső függvény – kötelező persistFlag
// // bool BarcodeRegistry::registerData_private(const BarcodeRecord& record, bool persistFlag) {
// //     if (!isBarcodeUnique(record.code)) {
// //         zWarning(QString("Barcode collision: %1 (%2)")
// //                      .arg(record.code, record.entityType));
// //         zEventWARN(QString("Barcode ütközés: %1 (%2)")
// //                        .arg(record.code, record.entityType));
// //         return false;
// //     }

// //     if (!add(record)) return false;

// //     if (IS_VERBOSE_THIS()) {
// //         zInfo(QString("Barcode registered: %1 [%2] {%3}")
// //                   .arg(record.entityType)
// //                   .arg(record.code)
// //                   .arg(OptionalUtils::toString(record.entityId)));
// //     }

// //     if (persistFlag) {
// //         persist(); // 🔧 csak ha kell
// //     }

// //     return true;
// // }

// // Publikus API – azonnali perzisztálás
// bool BarcodeRegistry::registerData(const BarcodeRecord& record) {
//     if (!add(record)) return false;
//     persist();
//     return true;

// }

// // Publikus API – volatile, nem perzisztál
// bool BarcodeRegistry::registerData_volatile(const BarcodeRecord& record) {
//     //return registerData_private(record, false);
//     return add(record);
// }

// bool BarcodeRegistry::retire(const QString& code, const QString& reason) {
//     for (auto& r : _items) {
//         if (r.code == code) {
//             if (r.retiredAt.has_value()) {
//                 zInfo(QString("Retire requested for already retired code: %1").arg(code));
//                 return false;
//             }

//             r.retiredAt = QDateTime::currentDateTime();
//             zInfo(QString("Barcode retired: %1 (reason=%2)").arg(code, reason));

//             persist();
//             return true;
//         }
//     }

//     zWarning(QString("Retire requested for unknown code: %1").arg(code));
//     return false;
// }

// // bool BarcodeRegistry::retire(const QString& code, const QString& reason) {
// //     // 🔍 Keresd meg a rekordot
// //     for (auto& r : _items) {
// //         if (r.code != code) continue;

// //         if (r.retiredAt.has_value()){
// //             zInfo(QString("Retire requested for already retired code: %1").arg(code));
// //             return false;
// //         }

// //         r.retiredAt = QDateTime::currentDateTime();

// //         zInfo(QString("Barcode retired: %1 (reason=%2)").arg(code, reason));

// //         persist();
// //         return true;
// //     }

// //     // ⚠️ Nem találtuk → audit warning
// //     zWarning(QString("Retire requested for unknown code: %1").arg(code));
// //     return false;
// // }

// void BarcodeRegistry::persist() const {
//     const QString path = FileNameHelper::instance().getBarcodeCsvFile();
//     if (path.isEmpty()) return;

//     BarcodeRepository::saveToCSV(*this, path);
// }
