#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/feature/register_me.h"
#include "common/registry/base/registry_engine_base.h"
#include "calculation/model/need_calculation_detail.h"
#include "common/registry/mixins/crud_mixin.h"
#include "common/registry/mixins/crud_workflow_mixin.h"


class NeedCalculationDetailRegistry
    : public RegistryEngineBase<NeedCalculationDetail>,
      public CrudMixin<NeedCalculationDetailRegistry, NeedCalculationDetail>,
      public CrudWorkflowMixin<NeedCalculationDetailRegistry, NeedCalculationDetail>,
      public RegisterMe<NeedCalculationDetailRegistry>
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


    bool insert(const NeedCalculationDetail& d) {
        return insertWithWorkflow(d);
    }

    bool update(const NeedCalculationDetail& d) {
        return updateWithWorkflow(d);
    }

    bool remove(const QUuid& id) {
        return removeWithWorkflow(id);
    }


    // --- Domain hookok ---
    bool beforeValidate(NeedCalculationDetail& d);
    bool validateDomain(const NeedCalculationDetail& d) const ;
    bool validateDuplicate(const NeedCalculationDetail& d) const ;

    bool beforeInsert(NeedCalculationDetail& d) ;
    bool beforeUpdate(NeedCalculationDetail& d) ;
    bool beforeRemove(NeedCalculationDetail&) { return true; }

    void afterInsert(const NeedCalculationDetail&) {}
    void afterUpdate(const NeedCalculationDetail&) {}
    void afterRemove(const NeedCalculationDetail&) {}

    void onInsertLog(const NeedCalculationDetail& d) ;
    void onUpdateLog(const NeedCalculationDetail& d) ;
    void onRemoveLog(const NeedCalculationDetail& d) ;

    void onLoadLog() ; // <- ÚJ
    void persist() const ;


private:
    NeedCalculationDetailRegistry()
        : RegistryEngineBase("NeedCalculationDetailRegistry", "NeedCalculationDetail")
    {}

    // Segédek
    static bool isFormulaValid(const QString& f);
    static bool materialExists(const QUuid& materialId);
};

