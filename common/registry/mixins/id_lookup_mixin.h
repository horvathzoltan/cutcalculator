#pragma once
#include "common/registry/barcode/id_lookup_registry_interface.h"

/**
 * 🧩 IdLookupInterfaceMixin<Host, Entity>
 *
 * Cél:
 * ----
 *  Ez a mixin egyetlen feladatot lát el: *implementálja* az
 *  IdLookupRegistryInterface-t a registryk számára.
 *
 *  A RegistryManager polimorf módon (runtime dynamic_cast segítségével)
 *  szeretné elérni az entitásokat kizárólag a typeName + QUuid alapján.
 *  Ehhez minden olyan registrynek, amely IdentifiableEntity-t tárol,
 *  biztosítania kell a findEntityById() metódust.
 *
 * Miért mixin?
 * ------------
 *  - A RegistryEngineBase már tartalmaz findById()-t → nem akarjuk duplikálni.
 *  - Az IdLookupRegistryInterface tiszta virtuális → implementálni kell.
 *  - A mixin biztosítja az implementációt *minden registry számára*,
 *    copy‑paste nélkül.
 *  - A mixin NEM ad hozzá semmilyen más API-t → nincs ambiguous, nincs zaj.
 *
 * Követelmények:
 * --------------
 *  - Host::findById(const QUuid&) létezzen és const legyen.
 *  - Entity származzon IdentifiableEntity-ből (ez a registrykben igaz).
 *
 * Mit csinál?
 * -----------
 *  - Meghívja a Host::findById(id)-t.
 *  - Ha talál entitást, visszaadja IdentifiableEntity* formában.
 *  - Ha nem talál, nullptr-t ad vissza.
 *
 * Mit NEM csinál?
 * ---------------
 *  - Nem duplikálja a readAll(), findAll(), findIf() API-kat.
 *  - Nem épít indexet.
 *  - Nem módosítja a registry tartalmát.
 *  - Nem workflow, nem persist, nem CRUD.
 *
 * Miért fontos?
 * -------------
 *  A RegistryManager::findEntity(typeName, id) kizárólag az
 *  IdLookupRegistryInterface-en keresztül működik. Ha egy registry
 *  nem implementálja ezt az interface-t, a UI több pontja (MaterialPicker,
 *  ProductTree, NeedRule szerkesztés, MatrixValidator) nem tudna
 *  entitást visszakeresni.
 *
 *  Ez a mixin tehát a polimorf ID‑lookup réteg tiszta, zajmentes,
 *  egyértelmű implementációja.
 */
template<typename Host, typename Entity>
struct IdLookupInterfaceMixin : public IdLookupRegistryInterface
{
    const IdentifiableEntity* findEntityById(const QUuid& id) const override
    {
        auto* self = static_cast<const Host*>(this);
        if (auto* e = self->findById(id))
            return static_cast<const IdentifiableEntity*>(e);
        return nullptr;
    }
};

