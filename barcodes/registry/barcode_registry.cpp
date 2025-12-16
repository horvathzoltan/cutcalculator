#include "barcodes/registry/barcode_registry.h"
#include "common/logger/logger.h"
#include "common/logger/event_logger.h"
#include "common/system/verbose_manager.h"
#include "barcodes/repository/barcode_repository.h"
#include "common/utils/filename_helper.h"
#include "common/utils/optional_utils.h"
#include "barcodes/helpers/barcode_collision_helper.h"

BarcodeRegistry& BarcodeRegistry::instance() {
    static BarcodeRegistry reg;
    return reg;
}

const BarcodeRecord* BarcodeRegistry::findByCode(const QString& code) const {
    for (const auto& r : _data)
        if (r.code == code)
            return &r;
    return nullptr;
}

bool BarcodeRegistry::isBarcodeUnique(const QString& code) const {
    for (const auto& r : _data)
        if (r.code == code)
            return false;
    return true;
}

const IdentifiableEntity* BarcodeRegistry::findEntityById(const QUuid& id) const {
    // for (const auto& r : _data) {
    //     if (r.entityId == id) {
    //         // implicit upcast BarcodeRecord → IdentifiableEntity (ha szükséges)
    //         return reinterpret_cast<const IdentifiableEntity*>(&r);
    //     }
    // }
    return nullptr;
}

bool BarcodeRegistry::registerNew(const QString& code,
                                  const QString& entityType,
                                  const QUuid& id,
                                  const QString& name)
{
    if (auto* rec = const_cast<BarcodeRecord*>(findByCode(code))) {
        // Már létezik → hazatérés
        if (!rec->entityId.has_value()) {
            rec->entityId = id; // frissítjük az entityId-t
            return true;
        } else {
            // Ha már van entityId és más → ütközés
            if (rec->entityId != id) {
                if (auto err = BarcodeCollisionHelper::makeBarcodeCollisionError(
                        entityType,
                        BarcodeCollisionHelper::RowInfo{ code, name },
                        /* lineNumber */ 0)) // ha nincs CSV sor, adhatsz 0-t vagy -1-et
                {
                    // audit log – ugyanazt az üzenetet használjuk
                    zWarning(err->errorMessage());
                    zEventWARN(err->errorMessage());
                }
                return false;
            }
            return true; // ugyanaz az entitás → oké
        }
    }

    // Új kód → rendes regisztráció
    BarcodeRecord rec;
    rec.code         = code;
    rec.entityType   = entityType;
    rec.entityId     = id;
    rec.introducedAt = QDateTime::currentDateTime();
    _data.append(rec);

    persist();
    return true;
}


// 🔐 Privát belső függvény – kötelező persistFlag
bool BarcodeRegistry::registerData_private(const BarcodeRecord& record, bool persistFlag) {
    if (!isBarcodeUnique(record.code)) {
        zWarning(QString("Barcode collision: %1 (%2)")
                     .arg(record.code, record.entityType));
        zEventWARN(QString("Barcode ütközés: %1 (%2)")
                       .arg(record.code, record.entityType));
        return false;
    }

    _data.append(record);

    if (IS_VERBOSE_THIS()) {
        zInfo(QString("Barcode registered: %1 [%2] {%3}")
                  .arg(record.entityType)
                  .arg(record.code)
                  .arg(OptionalUtils::toString(record.entityId)));
    }

    if (persistFlag) {
        persist(); // 🔧 csak ha kell
    }

    return true;
}

// Publikus API – azonnali perzisztálás
bool BarcodeRegistry::registerData(const BarcodeRecord& record) {
    return registerData_private(record, true);
}

// Publikus API – volatile, nem perzisztál
bool BarcodeRegistry::registerData_volatile(const BarcodeRecord& record) {
    return registerData_private(record, false);
}


bool BarcodeRegistry::retire(const QString& code, const QString& reason) {
    // 🔍 Keresd meg a rekordot
    for (auto& r : _data) {
        if (r.code != code) continue;

        if (r.retiredAt.has_value()){
            zWarning(QString("Retire requested for already retired code: %1").arg(code));
            return false;
        }

        r.retiredAt = QDateTime::currentDateTime();

        zWarning(QString("Barcode retired: %1 (reason=%2)").arg(code, reason));
        zEventINFO(QString("Barcode retired: %1 (reason=%2)").arg(code, reason));

        persist();
        return true;
    }

    // ⚠️ Nem találtuk → audit warning
    zWarning(QString("Retire requested for unknown code: %1").arg(code));
    return false;
}

void BarcodeRegistry::persist() const {
    const QString path = FileNameHelper::instance().getBarcodeCsvFile();
    if (path.isEmpty()) return;

    BarcodeRepository::saveToCSV(*this, path);
}
