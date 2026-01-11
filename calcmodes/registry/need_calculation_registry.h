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

    bool insert(const NeedCalculation& nc) {
        return insertWithWorkflow(nc);
    }

    bool update(const NeedCalculation& nc) {
        return updateWithWorkflow(nc);
    }

    bool remove(const QUuid& id) {
        return removeWithWorkflow(id);
    }

    bool validateDomain(const NeedCalculation& nc) const;
    bool validateDuplicate(const NeedCalculation& nc) const;

    bool beforeInsert(NeedCalculation&) { return true; }
    bool beforeUpdate(NeedCalculation&) { return true; }
    bool beforeRemove(NeedCalculation&) { return true; }

    void onInsertLog(const NeedCalculation& nc);
    void persist() const;
    void onLoadLog();

    void onUpdateLog(const NeedCalculation& nc);
    void onRemoveLog(const NeedCalculation& nc);

};



// /**
//  * @class NeedCalculationRegistry
//  * @brief Számítási módok (ModeName) per ProductDefinition – auditbarát tároló.
//  *
//  * *
//  * - a "productDefinitionId" per-modus csoportosítást jelent (egy termékhez több mód).
//  * - duplikációt tiltjuk: ugyanazon producton belül ugyanazzal a modeName-nel nem lehet 2 rekord.
//  */
// class NeedCalculationRegistry : public IdentifiableRegistryEngine<NeedCalculation> ,
//                                 public RegisterMe<NeedCalculationRegistry>
// {
//     AUTO_REGISTER_REGISTRY(NeedCalculationRegistry);


// private:
//     NeedCalculationRegistry();
// protected:
//     bool validateDomain(const NeedCalculation& c) const override;
//     bool validateDuplicate(const NeedCalculation& c) const override;
//     void onInsertLog(const NeedCalculation& c) override;
//     bool beforeUpdate(const NeedCalculation &c) override;
//     void onUpdateLog(const NeedCalculation &c) override;
// public:

//     static NeedCalculationRegistry& instance() {
//         static NeedCalculationRegistry inst;
//         return inst;
//     }

//     // CRUD
//     bool insert(const NeedCalculation& calc);
//     bool remove(const QUuid& id);
//     bool update(const NeedCalculation& calc);
//     bool rename(const QUuid& id, const QString& newName);

//     // Lookups
//     QVector<NeedCalculation> findByProduct(const QUuid& productId) const;
//     const NeedCalculation* findByProductAndName(const QUuid& productId, const QString& modeName) const;

//     // Persistence hooks (repo hívja/felhívva)
//     void persist() const override;
//     bool exists(const QUuid& productId, const QString& modeName) const;
// };
