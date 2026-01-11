#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/repository/need_calculation_repository.h"
#include "common/logger/logger.h"
// ⭐ Singleton
NeedCalculationRegistry& NeedCalculationRegistry::instance()
{
    static NeedCalculationRegistry inst;
    return inst;
}

// ⭐ Konstruktor – repository load + setAll()
NeedCalculationRegistry::NeedCalculationRegistry()
    : RegistryEngineBase("NeedCalculationRegistry", "NeedCalculation")
{
    // A betöltést a CalculationModesManager / StartupManager végzi, nem a konstruktor.
}

// ⭐ Kényelmi API – a NeedCalculator használja
const NeedCalculation*
NeedCalculationRegistry::findByProductAndName(const QUuid& productId,
                                              const QString& name) const
{
    return findIf([&](const NeedCalculation& c){
        return c.productId == productId && c.name == name;
    });
}

// --- Domain hookok ---

bool NeedCalculationRegistry::validateDomain(const NeedCalculation& nc) const
{
    return !nc.name.isEmpty();
}

bool NeedCalculationRegistry::validateDuplicate(const NeedCalculation& nc) const
{
    return !existsBy([&](const NeedCalculation& x){
        return x.productId == nc.productId
               && x.name == nc.name;
    });
}

// bool NeedCalculationRegistry::beforeInsert(NeedCalculation&)
// {
//     return true;
// }

// void NeedCalculationRegistry::afterInsert(const NeedCalculation&)
// {
// }

void NeedCalculationRegistry::onInsertLog(const NeedCalculation& nc)
{
    zInfo(QString("NeedCalculation inserted: %1").arg(nc.name));
}

void NeedCalculationRegistry::persist() const
{
    NeedCalculationRepository::save(readAll());
}

void NeedCalculationRegistry::onLoadLog()
{
    zInfo(QString("📊 NeedCalculationRegistry: %1 mód betöltve").arg(size()));
}

void NeedCalculationRegistry::onUpdateLog(const NeedCalculation& nc)
{
    zInfo(QString("NeedCalculation updated: %1").arg(nc.name));
}

void NeedCalculationRegistry::onRemoveLog(const NeedCalculation& nc)
{
    zInfo(QString("NeedCalculation removed: %1").arg(nc.name));
}
