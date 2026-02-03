#pragma once

// ⚠️ CrudWorkflowMixin szerződés:
// - ez a domain CRUD egyetlen helyes runtime útvonala
// - validate + duplicate + before* hookok + log + persist
// - MINDIG persistál
// - domain registryk insert/update/remove publikus API-ja erre kell épüljön

//#include "common/registry/mixins/crud_mixin.h"

// ⚠️ CrudWorkflowMixin MASTER CONTRACT:
//
// A domain CRUD egyetlen érvényes útvonala (insert/update/remove):
//   validate → storage (store*Impl + notifyItemsChanged) → log → persist → afterHook
//
// Engedélyezett lépések:
// - beforeValidate / validateDomain / validateDuplicate
// - store*Impl (memória)
// - persist() (workflow részeként)
// - on*Log (audit/diagnosztika)
// - after*Hook (opcionális, mellékhatás-mentes)
//
// Tiltások:
// - engine-level write API hívása TILOS (add/updateInternal/removeInternal/clear)
// - kézi persist() hívása TILOS
// - közvetlen memória-módosítás TILOS (store*Impl megkerülése)
// - CRUD hívása load-path alatt TILOS (setAll, konstruktor, CSV betöltés)
// - CRUD hívása konstruktorból/destruktorból TILOS
// - aszinkron vagy deferred CRUD TILOS
//
// Invariánsok:
// - minden lépés guardolt (guardInstanceUsage)
// - storage művelet után kötelező log + persist()
// - log mindig a persist ELŐTT fut
// - a workflow lépései sorrendben és megszakíthatatlanul futnak
// - a workflow nem hagyhat maga után részben alkalmazott módosítást
//
// Hibakezelés:
// - validate hibák → azonnali false, nincs módosítás
// - store*Impl hiba → nincs rollback (storage atomicitás)
// - persist hiba → memória módosult, tartós állapot nem; hívó fél dönt
// - log hibák elnyelése kötelező
// - workflow soha nem dob kivételt kifelé (false visszatérés)
//
// ⚠️ Host interface szerződés (CrudWorkflowMixin használatához):
// A Host-nak az alábbi metódusokat kell biztosítania:
// - bool validateDomain(Entity&)
// - bool validateDuplicate(Entity&)
// - bool beforeInsert(Entity&), bool beforeUpdate(Entity&), bool beforeRemove(Entity&)
// - void onInsertLog(const Entity&), onUpdateLog(const Entity&), onRemoveLog(const Entity&)
// - void persist()
// Ezek a metódusok:
// - nem dobhatnak kivételt kifelé
// - nem indíthatnak új CRUD-ot
// - nem kerülhetik meg a CrudWorkflowMixin szerződését
//
// ⚠️ CrudWorkflowMixin szerződés:
// - a domain CRUD egyetlen érvényes útvonala (insert/update/remove)
// - workflow: validate → storage (store*Impl + notifyItemsChanged) → log → persist → afterHook
// - NEM hív közvetlenül engine-level write API-t (add/updateInternal/removeInternal)
// - NEM használható registry konstruktorból vagy load-path alatt
// - minden persist hívás a workflow része, kézi persist TILOS
//
// ⚠️ Tiltások (CrudWorkflowMixin):
// - NEM hívhat engine-level write API-t (add/updateInternal/removeInternal/clear)
// - NEM kerülheti meg a store*Impl réteget (közvetlen memória-módosítás tilos)
// - NEM hívhat kézi persist()-et workflow-on kívül
// - NEM használható load-path alatt (setAll, konstruktor, CSV betöltés)
// - NEM hívható registry konstruktorból vagy destruktorból
// - NEM végezhet aszinkron vagy deferred CRUD műveletet

template<typename Host, typename Entity>
struct CrudWorkflowMixin {
    using IdType = typename Entity::IdType;

    // ⚠️ Workflow invariánsok:
    // - minden lépés guardolt (guardInstanceUsage)
    // - minden storage művelet store*Impl-en keresztül történik
    // - minden storage művelet után kötelező a megfelelő log hívás
    // - a log mindig a persist ELŐTT fut
    // - minden storage művelet után kötelező a persist()
    // - a workflow lépései sorrendben és megszakíthatatlanul futnak
    // - a workflow nem hagyhat maga után részben alkalmazott módosítást

    // ⚠️ Hibakezelési szerződés:
    // - bármely validate lépés hibája esetén a workflow azonnal false-szal visszatér
    // - storage művelet (store*Impl) hibája esetén nincs rollback, a művelet atomicitása a storage réteg felelőssége
    // - persist() hibája esetén a memória már módosult, de a tartós állapot nem; a hívó félnek kell kezelnie
    // - log hívások hibája nem szakíthatja meg a workflow-t
    // - a workflow soha nem dob kivételt kifelé; minden hiba false visszatéréssel jelzett

    // ⚠️ Persist szerződés:
    // - persist() kizárólag a workflow részeként hívható (insert/update/remove)
    // - minden store*Impl művelet után kötelezően egyszer fut
    // - kézi persist hívása TILOS (workflow-on kívül nincs tartósítás)
    // - a persist atomicitása a registry implementáció felelőssége
    // - persist hiba esetén a memória már módosult, a tartós állapot nem; a hívó fél dönt a kezelésről
    // - persist nem dobhat kivételt kifelé; hiba esetén false-t ad vissza

    // ⚠️ Log szerződés:
    // - minden storage művelet után kötelező a megfelelő log hívás (onInsertLog/onUpdateLog/onRemoveLog)
    // - a log mindig a persist ELŐTT fut
    // - a log hívások nem módosíthatják a memóriát vagy a tartós állapotot
    // - log hiba nem szakíthatja meg a workflow-t és nem adhat false-t
    // - a log réteg kizárólag audit/diagnosztikai célú, mellékhatás-mentes
    // - a log hívások sorrendje kötött: log → persist → afterHook

    // ⚠️ AfterHook szerződés:
    // - afterInsert/afterUpdate/afterRemove opcionális kiegészítő lépések
    // - nem módosíthatják a memóriát vagy a tartós állapotot
    // - nem hívhatnak CRUD-ot vagy persist-et
    // - nem szakíthatják meg a workflow-t (hiba esetén elnyelés kötelező)
    // - kizárólag UI/notification/side-channel jellegű műveletekre használhatók

    //régi:
    // ⚠️ Workflow-lépések (insert):
    // 1) beforeValidate
    // 2) validateDomain
    // 3) validateDuplicate
    // 4) storeAddImpl (memória)
    // 5) persist()
    // 6) onInsertLog
    // 7) afterInsert

    // új
    // ⚠️ Workflow-lépések (insert):
    // 1) validateDomain
    // 2) validateDuplicate
    // 3) beforeInsert
    // 4) storeAddImpl (memória + engine event) + notifyItemsChanged
    // 5) onInsertLog
    // 6) persist()


    bool insertWithWorkflow(Entity e) {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.validateDomain(e)) return false;
        if (!reg.validateDuplicate(e)) return false;
        if (!reg.beforeInsert(e)) return false;

        // --- Tényleges beszúrás: kifejezetten a CrudMixin insert() metódusa ---
        // if (!static_cast<CrudMixin<Host, Entity>&>(reg).insert(e))
        //     return false;

        // --- Storage: közvetlen storeAddImpl + engine event ---
        if (!reg.storeAddImpl(e))
            return false;
        reg.notifyItemsChanged();

        reg.onInsertLog(e);
        reg.persist();
        return true;
    }

    // ⚠️ Workflow-lépések (update):
    // 1) beforeValidate
    // 2) validateDomain
    // 3) validateDuplicate
    // 4) storeUpdateImpl (memória + engine event)
    // 5) persist()
    // 6) onUpdateLog
    // 7) afterUpdate

    // ⚠️ Workflow-lépések (update):
    // 1) validateDomain
    // 2) validateDuplicate
    // 3) beforeUpdate
    // 4) storeUpdateImpl (memória + engine event) + notifyItemsChanged
    // 5) onUpdateLog
    // 6) persist()


    bool updateWithWorkflow(Entity e) {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.validateDomain(e)) return false;
        if (!reg.validateDuplicate(e)) return false;
        if (!reg.beforeUpdate(e)) return false;

        // --- Tényleges update: kifejezetten a CrudMixin update() ---
        // if (!static_cast<CrudMixin<Host, Entity>&>(reg).update(e))
        //     return false;

        // --- Storage: közvetlen storeUpdateImpl + engine event ---
        if (!reg.storeUpdateImpl(e))
            return false;
        reg.notifyItemsChanged();


        reg.onUpdateLog(e);
        reg.persist();
        return true;
    }

    // ⚠️ Workflow-lépések (remove):
    // 1) beforeRemove
    // 2) storeRemoveImpl (memória)
    // 3) persist()
    // 4) onRemoveLog
    // 5) afterRemove

    // ⚠️ Workflow-lépések (remove):
    // 1) beforeRemove
    // 2) storeRemoveImpl (memória + engine event) + notifyItemsChanged
    // 3) onRemoveLog
    // 4) persist()


    bool removeWithWorkflow(const IdType& id) {
        Host& reg = static_cast<Host&>(*this);

        const Entity* existing = reg.findById(id);
        if (!existing) return false;

        Entity copy = *existing;

        if (!reg.beforeRemove(copy)) return false;

        // --- Tényleges remove: kifejezetten a CrudMixin remove() ---
        // if (!static_cast<CrudMixin<Host, Entity>&>(reg).remove(id))
        //     return false;


        // --- Storage: közvetlen storeRemoveImpl + engine event ---
        if (!reg.storeRemoveImpl(id))
            return false;
        reg.notifyItemsChanged();


        reg.onRemoveLog(copy);
        reg.persist();
        return true;
    }
};
