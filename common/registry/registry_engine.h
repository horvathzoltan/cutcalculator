#pragma once

#include "registry_base.h"
#include <QList>
#include <QUuid>
//#include "barcodes/registry/barcode_registry.h"
#include "common/system/verbose_manager.h"

template<typename T>
class RegistryEngine : public RegistryBase {
public:
    using EntityType = T;

    RegistryEngine(const QString& registryName,
                   const QString& entityTypeName)
        : RegistryBase(registryName, entityTypeName)
    {}

    // --- CRUD API ---
    bool add(const T& e) {
        guardInstanceUsage();
        // később: onBeforeAdd(e);
        _items.append(e);
        // később: onAfterAdd(e);
        return true;
    }

    bool update(const T& e) {
        guardInstanceUsage();
        for (int i = 0; i < _items.size(); ++i) {
            if (_items[i].id == e.id) {
                // később: onBeforeUpdate(_items[i]);
                _items[i] = e;
                // később: onAfterUpdate(e);
                return true;
            }
        }
        return false;
    }

    bool remove(const QUuid& id) {
        guardInstanceUsage();
        for (int i = 0; i < _items.size(); ++i) {
            if (_items[i].id == id) {
                // később: onBeforeRemove(_items[i]);
                _items.removeAt(i);
                // később: onAfterRemove(...);
                return true;
            }
        }
        return false;
    }

    // find
    const T* find(const QUuid& id) const {
        guardInstanceUsage();
        for (const auto& item : _items) {
            if (item.id == id)      // <-- id() helyett id
                return &item;
        }
        return nullptr;
    }


    QList<const T*> all() const {
        guardInstanceUsage();
        QList<const T*> out;
        out.reserve(_items.size());
        for (const auto& item : _items)
            out.append(&item);
        return out;
    }

    int size() const override {
        guardInstanceUsage();
        return _items.size();
    }

    // --- kompatibilitási API (régi kód miatt) ---

    // findById: régi API
    const T* findById(const QUuid& id) const {
        return find(id);
    }


    // findByBarcode: régi API
    //template<typename U = T>
    // findByBarcode
    const T* findByBarcode(const QString& barcode) const {
        guardInstanceUsage();
        for (const auto& item : _items) {
            if (item.barcode == barcode)   // <-- barcode() helyett barcode
                return &item;
        }
        return nullptr;
    }


    // readAll: régi API
    QList<T> readAll() const {
        guardInstanceUsage();
        return _items;
    }

    // bool registerData(const T& e) {
    //     guardInstanceUsage();
    //     // 1) BarcodeRegistry integráció – domain logika
    //     if constexpr (requires { e.barcode; e.id; e.name; }) {
    //         auto& barcodeRegistry = BarcodeRegistry::instance();

    //         if (!barcodeRegistry.registerNew(e.barcode, typeName(), e.id, e.name)) {
    //             return false; // audit már megtörtént
    //         }
    //     }

    //     // 2) CRUD add
    //     add(e);

    //     // 3) Verbose log (ha van name mező)
    //     if constexpr (requires { e.name; e.barcode; e.id; }) {
    //         if (IS_VERBOSE_THIS()) {
    //             zInfo(QString("Registered: %1 [%2] {%3}")
    //                       .arg(e.name)
    //                       .arg(e.barcode)
    //                       .arg(e.id.toString(QUuid::WithoutBraces)));
    //         }
    //     }

    //     return true;
    // }



protected:
    QList<T> _items;
};
