#include "needs/registry/need_rule_registry.h"
#include "needs/repository/need_rule_repository.h"

// --- Lookup API ---

NeedRuleRegistry::NeedRuleRegistry()
    : RegistryEngineBase("NeedRuleRegistry", "NeedRule")
{
    // A betöltést a StartupManager végzi, nem a konstruktor.
}



QVector<NeedRule>
NeedRuleRegistry::findByLeft(const QUuid& leftId) const
{
    return findAll([&](const NeedRule& r){
        return r.leftId == leftId;
    });
}

// --- Domain hookok ---

bool NeedRuleRegistry::validateConnection(const NeedRule& r) const
{
    // A ConnectionEntity már tud bal/jobb lookupot
    const bool leftOk  = r.left().has_value();
    const bool rightOk = r.right().has_value();
    return leftOk && rightOk;
}

bool NeedRuleRegistry::validateDuplicate(const NeedRule& r) const
{
    return !existsBy([&](const NeedRule& x){
        return x.leftId  == r.leftId
               && x.rightId == r.rightId;
    });
}


// --- Log hookok ---

void NeedRuleRegistry::onInsertLog(const NeedRule& r)
{
    zInfo(QString("➕ NeedRule INSERT: left=%1 right=%2")
              .arg(r.leftId.toString(), r.rightId.toString()));
}

void NeedRuleRegistry::onRemoveLog(const NeedRule& r)
{
    zInfo(QString("🗑️ NeedRule REMOVE: left=%1 right=%2")
              .arg(r.leftId.toString(), r.rightId.toString()));
}

// --- Persist ---

void NeedRuleRegistry::persist() const
{
    NeedRuleRepository::save();
}


