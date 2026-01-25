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

bool NeedCalculationRegistry::beforeValidate(NeedCalculation& nc) {
    nc.name = nc.name.trimmed();
    return true;
}


bool NeedCalculationRegistry::validateDomain(const NeedCalculation& nc) const
{
    return !nc.name.isEmpty();
}

bool NeedCalculationRegistry::validateDuplicate(const NeedCalculation& nc) const
{
    return !existsBy([&](const NeedCalculation& x){
        return x.productId == nc.productId
               && x.name == nc.name
               && x.id != nc.id;
    });
}


void NeedCalculationRegistry::persist() const
{
    NeedCalculationRepository::save(readAll());
}


void NeedCalculationRegistry::onInsertLog(const NeedCalculation& nc)
{
    zInfo(QString("➕ NeedCalculation INSERT: id=%1 name=%2")
              .arg(nc.id.toString(), nc.name));
}

void NeedCalculationRegistry::onUpdateLog(const NeedCalculation& nc)
{
    zInfo(QString("✏️ NeedCalculation UPDATE: id=%1 name=%2")
              .arg(nc.id.toString(), nc.name));
}

void NeedCalculationRegistry::onRemoveLog(const NeedCalculation& nc)
{
    zInfo(QString("🗑️ NeedCalculation REMOVE: id=%1 name=%2")
              .arg(nc.id.toString(), nc.name));
}


void NeedCalculationRegistry::onLoadLog()
{
    zInfo(QString("📊 NeedCalculationRegistry: %1 mód betöltve").arg(size()));
}
