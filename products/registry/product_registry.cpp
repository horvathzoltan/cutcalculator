// products/registry/product_registry.cpp
#include "products/registry/product_registry.h"
#include "common/utils/filename_helper.h"
#include "common/utils/scoped_per_thread_lock.h"
#include "products/repository/product_repository.h"

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

ProductMaster* ProductRegistry::findById(const QUuid& id) {
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
        if (pm.parentId.isNull() || !allIds.contains(pm.parentId)) {
            result.append(pm);
        }
    }

    return result;
}


// CRUD

void ProductRegistry::insert(const ProductMaster& pm) {
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
        _data.append(pm);
    }
    persist();
}

bool ProductRegistry::update(const ProductMaster& updated) {
    bool changed = false;
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), true);
        for (auto& pm : _data) {
            if (pm.id == updated.id) {
                pm = updated;   // teljes objektum cseréje
                changed = true;
                break;
            }
        }
    }
    if (changed) {
        persist();
        //zEvent(QString("✏️ Product updated: %1").arg(updated.name));
    }
    return changed;
}


bool ProductRegistry::remove(const QUuid& id) {
    bool removed = false;
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), true);
        for (int i = 0; i < _data.size(); ++i) {
            if (_data[i].id == id) {
                _data.remove(i);
                removed = true;
                break;
            }
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
