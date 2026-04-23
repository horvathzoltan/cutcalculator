// #pragma once

// #include <QVector>

// #include <common/registry/base/registry_engine_base.h>


// /**
//  * 🧩 SimpleLookupMixin<Host, Entity>
//  *
//  * Cél:
//  * ----
//  *  Egyetlen feladat: kényelmi lookup API biztosítása minden registry számára,
//  *  amely a RegistryEngineBase<Entity> alapfunkcióira épül.
//  *
//  * Mit ad?
//  * -------
//  *  - readAll()      → összes entitás
//  *  - findById(id)   → ID alapú lookup
//  *  - findBy(pred)   → predikátumos keresés (findAll wrapper)
//  *
//  * Miért jó?
//  * ---------
//  *  - Nem kell minden registrybe külön wrapper.
//  *  - Nem kell IdLookupMixin, ha nem kell interface.
//  *  - Nem kell BarcodeIndexMixin, ha nincs barcode.
//  *  - Tiszta, zajmentes, CRTP-kompatibilis.
//  *
//  * Követelmények:
//  * --------------
//  *  - Host örökölje a RegistryEngineBase<Entity>-t.
//  *  - Host::findAll(Predicate) létezzen.
//  */
// template<typename Host, typename Entity>
// struct SimpleLookupMixin
// {
//     // --- readAll() wrapper ---
//     QVector<Entity> readAll() const
//     {
//         auto* self = static_cast<const Host*>(this);
//         return self->RegistryEngineBase<Entity>::readAll();
//     }

//     // --- findById() wrapper ---
//     const Entity* findById(const QUuid& id) const
//     {
//         auto* self = static_cast<const Host*>(this);
//         return self->RegistryEngineBase<Entity>::findById(id);
//     }

//     // --- predikátumos keresés ---
//     template<typename Predicate>
//     QVector<Entity> findBy(Predicate&& pred) const
//     {
//         auto* self = static_cast<const Host*>(this);
//         return self->findAll(std::forward<Predicate>(pred));
//     }
// };
