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
        //      → hazatalálás (update), NEM CSV-dirty
        if (!rec->entityId.has_value()) {
            BarcodeRecord updated = *rec;
            updated.entityId = id;

            if (!updateInternal(updated)) {
                zWarning(QString("⚠️ Failed to attach entityId to existing barcode: %1")
                             .arg(trimmedCode));
                return false;
            }

            return true;   // hazatalálás → NEM CSV-dirty
        }

        // 1/B) Ugyanaz az entity → idempotens
        if (rec->entityId.value() == id) {
            if (rec->isActive())
                return true;   // idempotens, nincs változás
            else
                return false;  // retired → tiltott
        }

        // 1/C) Más entity → collision
        return false;
    }

    // 2) Új kód → insert → CSV-dirty
    BarcodeRecord rec;
    rec.id = QUuid::createUuid();
    rec.code = trimmedCode;
    rec.entityType = entityType;
    rec.entityId = id;
    rec.introducedAt = QDateTime::currentDateTime();

    if (!insertInternal(rec))
        return false;

    markCsvDirty();   // 🔥 új rekord → CSV-ben is látszik → dirty

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

        markCsvDirty();   // 🔥 CSV-ben látható változás


        zInfo(QString("Barcode retired: %1 (reason=%2)").arg(trimmedCode, reason));
        return true;   // 🔥 nincs persist
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


QStringList BarcodeRegistry::barcodesWithPrefix(const QString &prefix) const
{
    // Prefix normalizálása
    QString fixed = prefix;
    if (!fixed.endsWith(QLatin1Char('-')))
        fixed.append(QLatin1Char('-'));

    // findAll → QVector<BarcodeRecord>
    const QVector<BarcodeRecord> matches =
        findAll([&fixed](const BarcodeRecord &r) {
            return r.code.startsWith(fixed, Qt::CaseSensitive);
        });

    // Átalakítás QStringList-té
    QStringList out;
    out.reserve(matches.size());

    for (const BarcodeRecord &r : matches)
        out.append(r.code);

    return out;
}

bool BarcodeRegistry::persistIfDirty()
{
    if (!_csvDirty) {
        zInfo("📦 BarcodeRegistry::persistIfDirty → nincs változás, skip");
        return true;
    }

    zInfo("📦 BarcodeRegistry::persistIfDirty → változás detektálva, CSV mentése indul...");

    bool ok = BarcodeRepository::saveToCSV(*this);

    if (ok) {
        zInfo("📦 BarcodeRegistry::persistIfDirty → mentés sikeres, dirty flag törölve");
        _csvDirty = false;
    } else {
        zWarning("⚠️ BarcodeRegistry::persistIfDirty → mentés SIKERTELEN, dirty flag megmarad");
    }

    return ok;
}


