#pragma once

/**
 * 🧩 CrudWorkflowMixin – a domain entitások egyetlen érvényes CRUD‑útvonala
 *
 * Cél:
 * ----
 * - A domain registryk (Product, Material, NeedCalculation, NeedCalculationDetail)
 *   tiszta, auditálható, invariáns‑biztos CRUD‑workflowját biztosítja.
 * - A teljes CRUD‑folyamatot egységesíti: validáció → memória → log → persist.
 * - Megakadályozza az engine‑szintű write API megkerülését.
 *
 * Workflow lépések:
 * -----------------
 *   1) validateDomain
 *   2) validateDuplicate
 *   3) beforeInsert / beforeUpdate / beforeRemove
 *   4) store*Impl (memória + engine event) + notifyItemsChanged
 *   5) on*Log (audit/diagnosztika)
 *   6) persist()  (tartósítás)
 *   7) after*Hook (opcionális, mellékhatás‑mentes)
 *
 * Tiltások:
 * ---------
 * - Engine‑szintű write API hívása TILOS:
 *       add(), updateInternal(), removeInternal(), clear()
 * - Kézi persist() hívása workflow‑on kívül TILOS.
 * - Közvetlen memória‑módosítás TILOS (store*Impl megkerülése).
 * - CRUD hívása load‑path alatt TILOS (setAll, konstruktor, CSV betöltés).
 * - CRUD hívása konstruktorból vagy destruktorból TILOS.
 * - Aszinkron vagy deferred CRUD TILOS.
 *
 * Log szerződés:
 * --------------
 * - Minden storage művelet után kötelező a megfelelő log hívás.
 * - A log mindig a persist ELŐTT fut.
 * - A log nem módosíthat memóriát vagy tartós állapotot.
 *
 * Persist szerződés:
 * ------------------
 * - persist() kizárólag a workflow részeként hívható.
 * - persist() nem dobhat kivételt kifelé.
 * - persist() nem módosíthat memóriát és nem hívhat CRUD‑ot.
 *
 * AfterHook szerződés:
 * --------------------
 * - afterInsert/afterUpdate/afterRemove opcionális.
 * - Nem módosíthat memóriát vagy tartós állapotot.
 * - Nem hívhat CRUD‑ot vagy persist‑et.
 * - Mellékhatás‑mentes UI/diagnosztikai jelzésekre való.
 *
 * Összefoglaló:
 * -------------
 * Ez a mixin a domain CRUD egyetlen helyes útvonala.
 * Minden registry insert/update/remove művelete erre kell épüljön.
 * A workflow invariánsai megszakíthatatlanok és auditálhatók.
 */

template<typename Host, typename Connection>
struct ConnectionWorkflowMixin {

    using IdType = typename Connection::IdType;

    bool insertWithWorkflow(const IdType& leftId, const IdType& rightId) {
        Host& reg = static_cast<Host&>(*this);
        Connection c(leftId, rightId);

        if (!reg.validateConnection(c)) return false;
        if (!reg.validateDuplicate(c)) return false;
        if (!reg.beforeInsert(c)) return false;

        if (!reg.storeAddImpl(c)) return false;
        reg.notifyItemsChanged();

        reg.onInsertLog(c);
        reg.persist();
        return true;
    }

    bool removeWithWorkflow(const IdType& leftId, const IdType& rightId) {
        Host& reg = static_cast<Host&>(*this);

        const Connection* existing = reg.findByPair(leftId, rightId);
        if (!existing)
            return false;

        Connection copy = *existing;
        if (!reg.beforeRemove(copy)) return false;

        if (!reg.storeRemovePairImpl(leftId, rightId)) return false;
        reg.notifyItemsChanged();

        reg.onRemoveLog(copy);
        reg.persist();
        return true;
    }

    bool replaceWithWorkflow(const IdType& leftId,
                             const IdType& oldRightId,
                             const IdType& newRightId)
    {
        Host& reg = static_cast<Host&>(*this);

        // --- Find old connection ---
        const Connection* existing = reg.findByPair(leftId, oldRightId);
        if (!existing)
            return false;

        Connection oldCopy = *existing;
        Connection newConn(leftId, newRightId);

        if (!reg.validateConnection(newConn)) return false;
        if (!reg.validateDuplicate(newConn)) return false;

        if (!reg.beforeRemove(oldCopy)) return false;
        if (!reg.beforeInsert(newConn)) return false;

        if (!reg.storeRemovePairImpl(leftId, oldRightId)) return false;
        if (!reg.storeAddImpl(newConn)) return false;

        reg.notifyItemsChanged();

        reg.onRemoveLog(oldCopy);
        reg.onInsertLog(newConn);

        reg.persist();
        return true;
    }

};

