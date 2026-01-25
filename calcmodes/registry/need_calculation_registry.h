#pragma once

#include "calcmodes/model/need_calculation.h"
#include "common/registry/base/registry_engine_base.h"
#include "common/registry/feature/register_me.h"
#include "common/registry/mixins/crud_mixin.h"
#include "common/registry/mixins/crud_workflow_mixin.h"

class NeedCalculationRegistry
    : public RegistryEngineBase<NeedCalculation>,
      public CrudMixin<NeedCalculationRegistry, NeedCalculation>,
      public CrudWorkflowMixin<NeedCalculationRegistry, NeedCalculation>,
      public RegisterMe<NeedCalculationRegistry>
{
AUTO_REGISTER_REGISTRY

    public:
             static NeedCalculationRegistry& instance();
    NeedCalculationRegistry();

    const NeedCalculation* findByProductAndName(const QUuid& productId,
                                                const QString& name) const;

    bool insert(const NeedCalculation& nc) {return insertWithWorkflow(nc);}
    bool update(const NeedCalculation& nc) {return updateWithWorkflow(nc);}
    bool remove(const QUuid& id) {return removeWithWorkflow(id);}

    bool beforeValidate(NeedCalculation& nc);
    bool validateDomain(const NeedCalculation& nc) const;
    bool validateDuplicate(const NeedCalculation& nc) const;

    bool beforeInsert(NeedCalculation&) { return true; }
    bool beforeUpdate(NeedCalculation&) { return true; }
    bool beforeRemove(NeedCalculation&) { return true; }

    void afterInsert(const NeedCalculation&) {}
    void afterUpdate(const NeedCalculation&) {}
    void afterRemove(const NeedCalculation&) {}

    void onInsertLog(const NeedCalculation& nc);
    void persist() const;
    void onLoadLog();

    void onUpdateLog(const NeedCalculation& nc);
    void onRemoveLog(const NeedCalculation& nc);

};

