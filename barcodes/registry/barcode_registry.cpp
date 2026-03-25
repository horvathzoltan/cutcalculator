#include "barcodes/registry/barcode_registry.h"
#include "common/logger/logger.h"
#include "barcodes/repository/barcode_repository.h"

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

bool BarcodeRegistry::insertInternal(const BarcodeRecord& r)
{
    return storeAddImpl(r);
}

// bool BarcodeRegistry::updateInternal(const BarcodeRecord& r)
// {
//     return storeUpdateImpl(r);
// }

// --- Workflow hookok (ledger invariánsok érvényesítése) ---

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
    return !existsBy([&](const BarcodeRecord& x){
        return x.code == r.code && x.entityId != r.entityId;
    });
}

bool BarcodeRegistry::beforeInsert(BarcodeRecord& r)
{
    Q_UNUSED(r);
    // Jelenleg nincs extra logika – a caller (registerNew) tölti az introducedAt-et.
    return true;
}

bool BarcodeRegistry::beforeUpdate(BarcodeRecord& r)
{
    Q_UNUSED(r);
    // Itt is maradhatunk passzívak – a retire() állítja a retiredAt-et.
    return true;
}

// --- Log hookok ---

void BarcodeRegistry::onInsertLog(const BarcodeRecord& r)
{
    zInfo(QString("Barcode ledger insert: %1 [%2]").arg(r.entityType, r.code));
}

void BarcodeRegistry::onUpdateLog(const BarcodeRecord& r)
{
    zInfo(QString("Barcode ledger update: %1 [%2], status=%3")
              .arg(r.entityType,
                   r.code,
                   r.isActive() ? "Active" : "Retired"));
}


// törölve – ledger modellben nincs remove
// void BarcodeRegistry::onRemoveLog(const BarcodeRecord& r)
// {
//     zInfo(QString("Barcode removed from registry: %1 [%2]")
//               .arg(r.entityType, r.code));
// }

// --- Public API: registerNew / retire ---

// bool BarcodeRegistry::registerNew(const QString& code,
//                                   const QString& entityType,
//                                   const QUuid& id,
//                                   const QString& name)
// {
//     const QString trimmedCode = code.trimmed();
//     if (trimmedCode.isEmpty())
//         return false;

//     if (auto* rec = const_cast<BarcodeRecord*>(findByCode(trimmedCode))) {
//         // ÚJ TÖRVÉNY: ha létezik, TILTOTT – még akkor is, ha ugyanaz az entityId
//         // Non‑Reuse Identifier Policy-t valósít meg
//         return false;
//     }

//     // if (auto* rec = const_cast<BarcodeRecord*>(findByCode(trimmedCode))) {
//     //     // Már létezik ez a kód
//     //     if (!rec->entityId.has_value()) {
//     //         // Csak entityId hiányzik → frissítsük
//     //         BarcodeRecord updated = *rec;
//     //         updated.entityId = id;

//     //         if (!updateInternal(updated)) {
//     //             zWarning(QString("⚠️ Failed to update existing barcode: %1").arg(trimmedCode));
//     //             return false;
//     //         }

//     //         persist();
//     //         return true;
//     //     } else {
//     //         if (rec->entityId != id) {
//     //             zWarning(QString("Barcode collision: %1").arg(trimmedCode));
//     //             return false;
//     //         }

//     //         // // EntityId már van, és ha más, akkor ütközés
//     //         // if (rec->entityId != id) {
//     //         //      if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
//     //         //            entityType,
//     //         //            BarcodeCollisionHelper::RowInfo{ trimmedCode, name, id },
//     //         //            /* lineNumber */ 0))
//     //         //     {
//     //         //         zWarning(err->errorMessage());
//     //         //         zEventWARN(err->errorMessage());
//     //         //     }
//     //         //     return false;
//     //         // }
//     //         // // Ugyanaz az entitás → nincs teendő
//     //         return true;
//     //     }
//     // }

//     // Új kód → létrehozás
//     BarcodeRecord rec;
//     rec.id = QUuid::createUuid(); // 🔑 belső ID
//     rec.code         = trimmedCode;
//     rec.entityType   = entityType;
//     rec.entityId     = id;
//     rec.introducedAt = QDateTime::currentDateTime();

//     if (!insertInternal(rec)) {
//         zWarning(QString("⚠️ Failed to insert new barcode: %1").arg(trimmedCode));
//         return false;
//     }

//     persist();
//     return true;
// }

bool BarcodeRegistry::registerNew(const QString& code,
                                  const QString& entityType,
                                  const QUuid& id,
                                  const QString& name)
{
    const QString trimmedCode = code.trimmed();
    if (trimmedCode.isEmpty())
        return false;

    // 1) Létezik már ilyen kód?
    if (auto* rec = findByCode(trimmedCode)) {

        // 1/A) Félkész rekord (CSV-ből jött, nincs entityId)
        //      → ez NEM collision, hanem "hazatalálás"
        //      → befejezzük a rekordot: kitöltjük az entityId-t
        if (!rec->entityId.has_value()) {
            BarcodeRecord updated = *rec;
            updated.entityId = id;

            if (!updateInternal(updated)) {
                zWarning(QString("⚠️ Failed to attach entityId to existing barcode: %1")
                             .arg(trimmedCode));
                return false;
            }

            persist();
            return true;
        }

        // 1/B) Ugyanaz az entity → idempotens vagy retired tiltás
        if (rec->entityId.value() == id) {

            if (rec->isActive()) {
                // Idempotens: ugyanaz az entitás, aktív rekord → OK
                return true;
            } else {
                // Retired rekordot újra regisztrálni TILTOTT
                return false;
            }
        }

        // 1/C) Más entity → collision
        return false;
    }

    // 2) Új kód → teljesen új ledger rekord létrehozása
    BarcodeRecord rec;
    rec.id = QUuid::createUuid();              // belső technikai ID
    rec.code = trimmedCode;
    rec.entityType = entityType;
    rec.entityId = id;                         // már ismert entitás
    rec.introducedAt = QDateTime::currentDateTime();

    if (!insertInternal(rec))
        return false;

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

        if (!updateInternal(updated)) {
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
    // Egyelőre megtartjuk a meglévő API-t
    BarcodeRepository::saveToCSV(*this);
}
