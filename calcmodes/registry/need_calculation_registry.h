#pragma once

#include "calcmodes/model/need_calculation.h"
#include "common/registry/base/registry_engine.h"
#include "common/registry/workflow/crud_workflow_policy.h"

class NeedCalculationRegistry
    : public RegistryEngine<NeedCalculation, CrudWorkflowPolicy>
{
public:
    // ⭐ Singleton
    static NeedCalculationRegistry& instance();

    // ⭐ Konstruktor deklaráció (definíció a .cpp-ben)
    NeedCalculationRegistry();

    // ⭐ Kényelmi API – a NeedCalculator használja
    const NeedCalculation* findByProductAndName(const QUuid& productId,
                                                const QString& name) const;

protected:
    bool validateDomain(const NeedCalculation& nc) const override;
    bool validateDuplicate(const NeedCalculation& nc) const override;

    bool beforeInsert(const NeedCalculation&) override;
    void afterInsert(const NeedCalculation&) override;

    void onInsertLog(const NeedCalculation& nc) override;

    void persist() const override;
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
