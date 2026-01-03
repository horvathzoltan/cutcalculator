#pragma once
#include "barcodes/registry/barcode_registry.h"
#include "common/model/barcode_identifiable_entity.h"
#include "common/registry/base/identifiable_registry_helper.h"
//#include "common/logger/logger.h"

struct BarcodeRegistryHelper {
    template<typename RegistryT, typename EntityT>
    static bool insert(RegistryT& registry, const EntityT& entity) {
        // 1) BarcodeRegistry integráció
        // ha nem lehet barcodet csinálni hozzá, eldobjuk
        if (!BarcodeRegistryHelper::registerEntity(entity, registry.typeName()))
            return false;

        return IdentifiableRegistryHelper::insert(registry, entity);
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

private:
    template<typename T>
    static bool registerEntity(const T& entity, const QString& typeName) {
        auto& binstance = BarcodeRegistry::instance();

        bool ok = binstance.registerNew(entity.barcode,
                                        typeName,
                                        entity.id, entity.name);
        return ok;
    }
};



