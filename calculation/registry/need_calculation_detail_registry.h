#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/feature/register_me.h"
#include "common/registry/base/registry_engine_base.h"
#include "calculation/model/need_calculation_detail.h"
#include "common/registry/mixins/bulk_load_mixin.h"
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

class NeedCalculationDetailRegistry
    : public RegistryEngineBase<NeedCalculationDetail>,
      public CrudWorkflowMixin<NeedCalculationDetailRegistry, NeedCalculationDetail>,
      public RegisterMe<NeedCalculationDetailRegistry>,
      public TestSupportMixin<NeedCalculationDetailRegistry>,
      public BulkLoadMixin<NeedCalculationDetailRegistry, NeedCalculationDetail>
{
    AUTO_REGISTER_REGISTRY
public:
    static NeedCalculationDetailRegistry& instance() {
        static NeedCalculationDetailRegistry inst;
        return inst;
    }

    // Kényelmi API: formula módosítása → update workflow
    bool updateFormula(const QUuid& id, const QString& newFormula);

    // Lookup API
    QVector<NeedCalculationDetail> findByCalculation(const QUuid& calcId) const;


// ⚠️ Persist‑policy (WF‑3):
// - persist() kizárólag a workflow részeként hívható
// - nem módosíthat memóriát és nem hívhat CRUD‑ot
// - csak a repository‑t hívhatja (tiszta tartósítás)
// - nem dobhat kivételt kifelé
    bool insert(const NeedCalculationDetail& d);
    bool update(const NeedCalculationDetail& d);
    bool remove(const QUuid& id);

    bool validateDomain(const NeedCalculationDetail& d) const ;
    bool validateDuplicate(const NeedCalculationDetail& d) const ;

    bool beforeInsert(NeedCalculationDetail& d);
    bool beforeUpdate(NeedCalculationDetail& d);

    bool beforeRemove(NeedCalculationDetail&) { return true; }

    // --- AfterHook-ok ---
    // Mellékhatás-mentes, opcionális UI/notification lépések.
    // A workflow sorrendje: log → persist → afterHook.
    void afterInsert(const NeedCalculationDetail&);
    void afterUpdate(const NeedCalculationDetail&);
    void afterRemove(const NeedCalculationDetail&);

    void onInsertLog(const NeedCalculationDetail& d) ;
    void onUpdateLog(const NeedCalculationDetail& d) ;
    void onRemoveLog(const NeedCalculationDetail& d) ;

    void onLoadLog() ; // <- ÚJ
    void persist() const ;

    // Segédek

    // v2: formula rules → empty=valid, "unknown"=invalid, prefix-based syntax
    static bool isFormulaValid(const QString& f);
private:
    NeedCalculationDetailRegistry()
        : RegistryEngineBase("NeedCalculationDetailRegistry", "NeedCalculationDetail")
    {}

    bool validateMaterial(const NeedCalculationDetail& d);
    static bool materialExists(const QUuid& materialId);
    static bool validateOpt(const QString& f);
    static bool validateChoose(const QString& f);

};

