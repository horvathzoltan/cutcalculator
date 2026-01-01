#pragma once
#include <QVector>
#include <optional>
#include "common/registry/base/registry_base.h"
#include "common/registry/manager/registry_manager.h"
//#include "common/logger/event_logger.h"
#include "calculation/model/need_calculation_detail.h"

/**
 * @class NeedCalculationDetailRegistry
 * @brief Egy számítási mód részletei: Material + formula sorok.
 *
 * Hunglish:
 * - minden detail egy needCalculationId-hoz tartozik.
 * - validáció: materialId érvényes kell legyen (MaterialRegistry-ben), formula nem üres.
 */
class NeedCalculationDetailRegistry : public RegistryBase {
public:
    static NeedCalculationDetailRegistry& instance() {
        static NeedCalculationDetailRegistry inst;
        return inst;
    }

    int size() const override { return _data.size(); }
    const QVector<NeedCalculationDetail>& readAll() const { return _data; }

    // CRUD
    bool insert(const NeedCalculationDetail& det);
    bool remove(const QUuid& id);
    bool updateFormula(const QUuid& id, const QString& newFormula);

    // Lookups
    QVector<NeedCalculationDetail> findByCalculation(const QUuid& calcId) const;
    std::optional<NeedCalculationDetail> findById(const QUuid& id) const;

    // Persistence
    void setAll(const QVector<NeedCalculationDetail>& v) { _data = v; }
    void clear() { _data.clear(); }
    void persist() const;

private:
    NeedCalculationDetailRegistry()
        : RegistryBase("NeedCalculationDetailRegistry", "NeedCalculationDetail") {
        RegistryManager::instance().registerRepo(this);
    }

    QVector<NeedCalculationDetail> _data;

    static bool isFormulaValid(const QString& f);
};
