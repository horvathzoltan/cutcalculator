#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/base/identifiable_registry_engine.h"
#include "calculation/model/need_calculation_detail.h"
#include "common/registry/feature/register_me.h"

// NeedCalculationDetailRegistry
// - IdentifiableRegistryEngine<NeedCalculationDetail> alapján
// - CRUD: insert/update/remove → workflow
// - lookup: findByCalculation()
// - persist: repo.save(_items)

class NeedCalculationDetailRegistry
    : public IdentifiableRegistryEngine<NeedCalculationDetail>,
      public RegisterMe<NeedCalculationDetailRegistry>
{
    AUTO_REGISTER_REGISTRY(NeedCalculationDetailRegistry);

public:
    using Base = IdentifiableRegistryEngine<NeedCalculationDetail>;
    using EntityType = NeedCalculationDetail;

    static NeedCalculationDetailRegistry& instance() {
        static NeedCalculationDetailRegistry inst;
        return inst;
    }

    // --- Public API ---

    // CRUD – mind workflow-ra épül
    bool insert(const NeedCalculationDetail& det);
    bool update(const NeedCalculationDetail& det);
    bool remove(const QUuid& id);

    // Kényelmi API: csak formulanév módosítása → update workflow
    bool updateFormula(const QUuid& id, const QString& newFormula);

    // Lookups
    QVector<NeedCalculationDetail> findByCalculation(const QUuid& calcId) const;

    // Persist – engine hook
    void persist() const override;

protected:
    // --- Hookok ---

    bool beforeInsert(const NeedCalculationDetail& d) override;
    bool beforeUpdate(const NeedCalculationDetail& d) override;

    void onInsertLog(const NeedCalculationDetail& d) override;
    void onUpdateLog(const NeedCalculationDetail& d) override;
    void onRemoveLog(const NeedCalculationDetail& d) override;

private:
    NeedCalculationDetailRegistry();

    static bool isFormulaValid(const QString& f);
    static bool materialExists(const QUuid& materialId);
};
