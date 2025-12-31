#pragma once
#include "barcodes/registry/barcode_registry.h"
//#include "common/model/entity_format_helper.h"

struct BarcodeRegistryHelper {
    template<typename T>
    static bool registerEntity(const T& e, const QString& typeName) {
        auto& barcodeRegistry = BarcodeRegistry::instance();

        if (!barcodeRegistry.registerNew(e.barcode, typeName, e.id, e.name)) {
            return false; // audit már megtörtént
        }

        return true;
    }

    template<typename RegistryT, typename EntityT>
    static bool insert(RegistryT& registry, const EntityT& e) {
        // 1) BarcodeRegistry integráció
        if (!BarcodeRegistryHelper::registerEntity(e, registry.typeName()))
            return false;

        // 2) CRUD add
        registry.add(e);

        // 3) Verbose log
        if (registry.verbose()) {
            zInfo(toString(e, registry.typeName()));
        }

        return true;
    }

    static QString displayName(const BarcodeIdentifiableEntity& e,
                               const QString& typeName)
    {

        return QString("%1:%2") .arg(typeName, e.displayName());
    }

    static QString toString(const BarcodeIdentifiableEntity& e,
                            const QString& typeName)
    {
        return QString("%1:%2") .arg(typeName, e.toString());
    }
};



