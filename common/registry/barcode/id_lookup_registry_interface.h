#pragma once
#include "common/model/identifiable_entity.h"

/**
 * 🧩 IdLookupRegistryInterface
 *
 * Cél:
 * ----
 *  Polimorf (runtime) ID-alapú entitáskeresés biztosítása a RegistryManager
 *  számára. A RegistryManager csak a typeName-et és a QUuid-et ismeri,
 *  ezért szüksége van egy egységes, virtuális API-ra.
 *
 * Használat:
 * ----------
 *  - A registryk öröklik ezt az interface-t (közvetlenül vagy mixin-en át).
 *  - A RegistryManager dynamic_cast-tal ellenőrzi, hogy a registry
 *    támogatja-e az ID-alapú lookupot.
 *
 * Miért nem a RegistryEngineBase része?
 * -------------------------------------
 *  - A RegistryEngineBase nem tudja garantálni, hogy az entitás
 *    IdentifiableEntity-ből származik.
 *  - A RegistryEngineBase nem polimorf lookup réteg, csak storage engine.
 *
 * Megjegyzés:
 * -----------
 *  A legtöbb registry a IdLookupInterfaceMixin-en keresztül implementálja
 *  ezt a metódust, így nem kell minden registryben külön megírni.
 */
class IdLookupRegistryInterface {
public:
    virtual ~IdLookupRegistryInterface() = default;
    virtual const IdentifiableEntity* findEntityById(const QUuid&) const = 0;
};
