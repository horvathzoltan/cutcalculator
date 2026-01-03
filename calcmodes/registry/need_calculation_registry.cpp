#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/repository/need_calculation_repository.h"
#include "common/logger/logger.h"
#include "common/registry/base/identifiable_registry_helper.h"


NeedCalculationRegistry::NeedCalculationRegistry()
    : IdentifiableRegistryEngine<NeedCalculation>("NeedCalculationRegistry", "NeedCalculation")
{}

bool NeedCalculationRegistry::exists(const QUuid& productId, const QString& modeName) const {
    return existsBy([&](const NeedCalculation& c){
        return c.productId == productId &&
               c.name.compare(modeName, Qt::CaseInsensitive) == 0;
    });

}

bool NeedCalculationRegistry::validateDomain(const NeedCalculation& c) const {
    if (c.name.trimmed().isEmpty()) {
        zWarning("NeedCalculation: üres név tiltva");
        return false;
    }
    return true;
}

bool NeedCalculationRegistry::validateDuplicate(const NeedCalculation& c) const {
    return !existsBy([&](const NeedCalculation& x){
        return x.productId == c.productId &&
               x.name.compare(c.name, Qt::CaseInsensitive) == 0;
    });
}

void NeedCalculationRegistry::onInsertLog(const NeedCalculation& c) {
    logEntityAction("INSERT", c,
                    QString("productId=%1").arg(c.productId.toString()));
}

bool NeedCalculationRegistry::insert(const NeedCalculation& calc) {
    return IdentifiableRegistryHelper::insert(*this, calc);
}

bool NeedCalculationRegistry::remove(const QUuid& id) {
    return IdentifiableRegistryHelper::remove(*this, id);
}


bool NeedCalculationRegistry::beforeUpdate(const NeedCalculation& c) {
    if (c.name.trimmed().isEmpty()) {
        zWarning("NeedCalculation: üres név tiltva");
        return false;
    }

    if (exists(c.productId, c.name)) {
        zWarning(QString("NeedCalculation: duplikáció %1").arg(c.name));
        return false;
    }

    return true;
}


void NeedCalculationRegistry::onUpdateLog(const NeedCalculation& c) {
    logEntityAction("UPDATE", c,
                    QString("productId=%1").arg(c.productId.toString()));
}

bool NeedCalculationRegistry::update(const NeedCalculation& c) {
    return IdentifiableRegistryHelper::update(*this, c);
}


bool NeedCalculationRegistry::rename(const QUuid& id, const QString& newName) {
    auto opt = findById(id);
    if (!opt)
        return false;

    NeedCalculation updated = *opt;
    updated.name = newName;

    return update(updated);   // <-- workflow indul
}

QVector<NeedCalculation> NeedCalculationRegistry::findByProduct(const QUuid& productId) const {
    return findAll([&](const auto& c){
        return c.productId == productId;
    });
}

const NeedCalculation* NeedCalculationRegistry::findByProductAndName(
    const QUuid& productId,
    const QString& modeName) const
{
    return findIf([&](const NeedCalculation& c){
        return c.productId == productId && c.name.compare(modeName, Qt::CaseInsensitive) == 0;
    });
}

void NeedCalculationRegistry::persist() const {
    NeedCalculationRepository::save(_items);
}
