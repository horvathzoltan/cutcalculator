#pragma once
#include <QVector>
#include <optional>
#include "common/registry/base/registry_base.h"
#include "common/registry/manager/registry_manager.h"
#include "common/logger/event_logger.h"
#include "calcmodes/model/need_calculation.h"

/**
 * @class NeedCalculationRegistry
 * @brief Számítási módok (ModeName) per ProductDefinition – auditbarát tároló.
 *
 * Hunglish:
 * - a "productDefinitionId" per-modus csoportosítást jelent (egy termékhez több mód).
 * - duplikációt tiltjuk: ugyanazon producton belül ugyanazzal a modeName-nel nem lehet 2 rekord.
 */
class NeedCalculationRegistry : public RegistryBase {
public:
    static NeedCalculationRegistry& instance() {
        static NeedCalculationRegistry inst;
        inst.guardInstanceUsage();
        return inst;
    }

    int size() const override { return _data.size(); }

    const QVector<NeedCalculation>& readAll() const { return _data; }

    // CRUD
    bool insert(const NeedCalculation& calc);
    bool remove(const QUuid& id);
    bool rename(const QUuid& id, const QString& newName);

    // Lookups
    std::optional<NeedCalculation> findById(const QUuid& id) const;
    QVector<NeedCalculation> findByProduct(const QUuid& productId) const;
    std::optional<NeedCalculation> findByProductAndName(const QUuid& productId, const QString& modeName) const;

    // Persistence hooks (repo hívja/felhívva)
    void setAll(const QVector<NeedCalculation>& v) { _data = v; }
    void clear() { _data.clear(); }
    void persist() const;
    bool exists(const QUuid& productId, const QString& modeName) const;

private:
    NeedCalculationRegistry()
        : RegistryBase("NeedCalculationRegistry", "NeedCalculation") {
        RegistryManager::instance().registerRepo(this);
    }

    QVector<NeedCalculation> _data;
};
