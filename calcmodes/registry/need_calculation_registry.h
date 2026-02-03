#pragma once

#include "calcmodes/model/need_calculation.h"
#include "common/registry/base/registry_engine_base.h"
#include "common/registry/feature/register_me.h"
//#include "common/registry/mixins/crud_mixin.h"
#include "common/registry/mixins/crud_workflow_mixin.h"
#include "common/registry/mixins/test_support_mixin.h"

// ⚠️ CRUD WORKFLOW – VÉGLEGES SZERZŐDÉS (WF-4)
//
// A domain CRUD egyetlen érvényes útvonala (insert/update/remove):
//   validateDomain
//   → validateDuplicate
//   → beforeInsert / beforeUpdate / beforeRemove
//   → store*Impl + notifyItemsChanged
//   → on*Log
//   → persist()
//   → after*Hook (opcionális, mellékhatás-mentes)
//
// Kötelező invariánsok:
// - nincs engine-level write API hívás (add/updateInternal/removeInternal/clear)
// - nincs kézi persist() hívás workflow-on kívül
// - nincs közvetlen memória-módosítás store*Impl megkerülésével
// - nincs CRUD hívás load-path alatt (setAll, konstruktor, CSV betöltés)
// - nincs CRUD hívás konstruktorból/destruktorból
// - nincs aszinkron vagy deferred CRUD
//
// A workflow lépései megszakíthatatlanok, sorrendkötöttek, és nem hagyhatnak
// részleges állapotot. A log mindig a persist ELŐTT fut.


class NeedCalculationRegistry
    : public RegistryEngineBase<NeedCalculation>,
      public CrudWorkflowMixin<NeedCalculationRegistry, NeedCalculation>,
      public RegisterMe<NeedCalculationRegistry>,
      public TestSupportMixin<NeedCalculationRegistry>
{
AUTO_REGISTER_REGISTRY

public:
    static NeedCalculationRegistry& instance();
    NeedCalculationRegistry();

    const NeedCalculation* findByProductAndName(const QUuid& productId,
                                                const QString& name) const;


// ⚠️ Persist‑policy (WF‑3):
// - persist() kizárólag a workflow részeként hívható
// - nem módosíthat memóriát és nem hívhat CRUD‑ot
// - csak a repository‑t hívhatja (tiszta tartósítás)
// - nem dobhat kivételt kifelé

    bool insert(const NeedCalculation& nc);
    bool update(const NeedCalculation& nc);
    bool remove(const QUuid& id);

    bool validateDomain(const NeedCalculation& nc) const;
    bool validateDuplicate(const NeedCalculation& nc) const;

    bool beforeInsert(NeedCalculation&);
    bool beforeUpdate(NeedCalculation&);
    bool beforeRemove(NeedCalculation&) { return true; }

    // A workflow sorrendje: log → persist → afterHook.
    void afterInsert(const NeedCalculation&) {}
    void afterUpdate(const NeedCalculation&) {}
    void afterRemove(const NeedCalculation&) {}

    void onInsertLog(const NeedCalculation& nc);
    void persist() const;
    void onLoadLog();

    void onUpdateLog(const NeedCalculation& nc);
    void onRemoveLog(const NeedCalculation& nc);

};

