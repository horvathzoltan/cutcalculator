#pragma once

#include <QString>

/**
 * 🧩 BarcodeLookupMixin<Derived, Entity>
 *
 * Feltételek:
 *  - Derived örököl RegistryEngineBase<Entity>-ből (vagy kompatibilis basisból)
 *  - az Entity rendelkezik `barcode` mezővel
 *  - a Derived elérhetővé teszi a `findIf` metódust
 *
 * Eredmény:
 *  - egységes, típusbiztos `findByBarcode(const QString&)` API
 */
template<typename Derived, typename Entity>
class BarcodeLookupMixin {
public:
    const Entity* findByBarcode(const QString& barcode) const
    {
        auto* self = static_cast<const Derived*>(this);
        return self->findIf([&](const Entity& e){
            return e.barcode.compare(barcode, Qt::CaseInsensitive) == 0;
        });
    }
};
