// products/registry/product_registry.cpp
#include "products/registry/product_registry.h"
#include "common/utils/filename_helper.h"
#include "common/utils/scoped_per_thread_lock.h"
#include "products/repository/product_repository.h"
#include "common/registry/barcode_table.h"
#include "common/logger/event_logger.h"
#include "barcodes/registry/barcode_registry.h"

ProductRegistry& ProductRegistry::instance() {
    static ProductRegistry inst;
    return inst;
}

void ProductRegistry::setData(const QVector<ProductMaster>& v) {
    _data = v;
}

const QVector<ProductMaster>& ProductRegistry::readAll() const {
    return _data;
}

const ProductMaster* ProductRegistry::findById(const QUuid& id) const {
    for (auto& def : _data) {
        if (def.id == id) return &def;
    }
    return nullptr;
}

ProductMaster* ProductRegistry::findByBarcode(const QString& barcode) {
    for (auto& def : _data) {
        if (def.barcode == barcode) return &def;
    }
    return nullptr;
}

QVector<ProductMaster> ProductRegistry::findChildren(const QUuid& parentId) const {
    QVector<ProductMaster> children;
    for (const auto& def : _data) {
        if (def.parentId == parentId) children.append(def);
    }
    return children;
}

QVector<ProductMaster> ProductRegistry::roots() const {
    QVector<ProductMaster> result;

    QSet<QUuid> allIds;
    for (const auto& pm : _data) {
        allIds.insert(pm.id);
    }

    for (const auto& pm : _data) {
        if (pm.parentId.isNull() ) {//|| !allIds.contains(pm.parentId)
            result.append(pm);
        }
    }

    return result;
}


// CRUD

void ProductRegistry::insert(const ProductMaster& pm) {
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);

        auto& barcodeRegistry = BarcodeRegistry::instance();

        // ✅ A BarcodeRegistry::registerNew maga ellenőrzi és auditál
        if (!barcodeRegistry.registerNew(pm.barcode, typeName(), pm.id)) {
            // Audit WARN már a BarcodeRegistry-ben megtörtént
            return; // Strict model: nem kerül be
        }

        _data.append(pm);
    }
    persist();
}

bool ProductRegistry::update(const ProductMaster& updated) {
    bool changed = false;
    ScopedPerThreadLock locker(static_cast<void*>(&_mutex), true);

    auto& barcodeRegistry = BarcodeRegistry::instance();

    for (auto& pm : _data) {
        if (pm.id == updated.id) {
            // 🔍 Ha változott a barcode, kezeljük az életciklust
            if (pm.barcode != updated.barcode) {
                // A BarcodeRegistry::registerNew maga ellenőrzi a uniqueness-et és auditál
                if (!barcodeRegistry.registerNew(updated.barcode, typeName(), updated.id)) {
                    // Audit WARN már a BarcodeRegistry-ben megtörtént
                    return false;
                }

                // Nyugdíjazzuk a régi barcode-ot auditbarát módon
                barcodeRegistry.retire(pm.barcode, typeName() + " updated");
            }

            pm = updated;   // teljes objektum cseréje
            changed = true;
            break;
        }
    }

    if (changed) {
        persist();
        zEventINFO(QString("✏️ %3 updated: %1 [%2]")
                       .arg(updated.name, updated.barcode, typeName()));
    }
    return changed;
}

bool ProductRegistry::remove(const QUuid& id) {
    bool removed = false;
    ScopedPerThreadLock locker(static_cast<void*>(&_mutex), true);

    auto& barcodeRegistry = BarcodeRegistry::instance();

    for (int i = 0; i < _data.size(); ++i) {
        if (_data[i].id == id) {
            const auto code = _data[i].barcode;
            _data.remove(i);
            removed = true;

            // 🔧 Barcode retire
            barcodeRegistry.retire(code, typeName() + " deleted");
            zEventINFO(QString("%2 removed, barcode retired: %1").arg(code, typeName()));
            break;
        }
    }

    if (removed) persist();
    return removed;
}




bool ProductRegistry::isBarcodeUnique(const QString& barcode, const QUuid& excludeId) const {
    for (const auto& def : _data) {
        if (def.id != excludeId && def.barcode == barcode) {
            return false;
        }
    }
    return true;
}

void ProductRegistry::persist() const {
    const QString path = FileNameHelper::instance().getProductCsvFile();
    if (path.isEmpty()) return;

    QVector<ProductMaster> toWrite;
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
        toWrite = _data; // másolat, _data változatlan marad
    } // lock feloldva itt

    ProductRepository::saveToCSV(toWrite, path);
}

const IdentifiableEntity* ProductRegistry::findEntityById(const QUuid &id) const
{
    if (auto* pm = findById(id)) {
        return pm; // implicit upcast ProductMaster* → IdentifiableEntity*
    }
    return nullptr;
}
