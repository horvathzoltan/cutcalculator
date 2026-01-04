#include "needs/registry/need_rule_registry.h"

// --- Lookup API ---

NeedRuleRegistry::NeedRuleRegistry()
    : RegistryEngine("NeedRuleRegistry", "NeedRule")
{
    // QVector<NeedRule> tmp;
    // NeedRuleRepository::load(tmp);
    // setAll(tmp);
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

bool NeedRuleRegistry::beforeInsert(const NeedRule&)
{
    // Nincs extra before‑logika jelenleg
    return true;
}

void NeedRuleRegistry::afterInsert(const NeedRule&)
{
    // Jelenleg no‑op, de itt lehetne cache, index, stb.
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


// #include "needs/registry/need_rule_registry.h"
// #include "needs/repository/need_rule_repository.h"

// NeedRuleRegistry& NeedRuleRegistry::instance() {
//     static NeedRuleRegistry inst;
//     // FONTOS: itt NEM guardolunk, csak init-et kérünk
//     inst.initialize();   // RegistryBase-ből, egyszeri regisztráció a RegistryManager felé
//     return inst;
// }

// NeedRuleRegistry::NeedRuleRegistry()
//     : ConnectionRegistryEngine<NeedRule>("NeedRuleRegistry", "NeedRule")
// {}

// // Domain API
// void NeedRuleRegistry::insert(const NeedRule& rule) {
//     ConnectionRegistryEngine<NeedRule>::insert(rule);
//     persist();
// }

// bool NeedRuleRegistry::remove(const QUuid& productId, const QUuid& materialId) {
//     bool ok = ConnectionRegistryEngine<NeedRule>::remove(productId, materialId);
//     if (ok)
//         persist();
//     return ok;
// }

// void NeedRuleRegistry::persist() const {
//     NeedRuleRepository::save();
// }


// #include "needs/registry/need_rule_registry.h"
// #include "needs/repository/need_rule_repository.h"

// NeedRuleRegistry& NeedRuleRegistry::instance() {
//     static NeedRuleRegistry inst;
//     inst.guardInstanceUsage();
//     return inst;
// }

// // QString NeedRuleRegistry::typeName() const {
// //     return "NeedRule";
// // }

// int NeedRuleRegistry::size() const {
//     // közvetlenül a ConnectionRegistry-ből örökölt size()
//     return ConnectionRegistry<NeedRule>::size();
// }

// void NeedRuleRegistry::insert(const NeedRule& rule) {
//     // örökölt insert → bővítve perzisztálással
//     ConnectionRegistry<NeedRule>::insert(rule);
//     persist();
// }

// bool NeedRuleRegistry::remove(const QUuid& productId, const QUuid& materialId) {
//     bool ok = ConnectionRegistry<NeedRule>::remove(productId, materialId);
//     if (ok) persist();
//     return ok;
// }

// void NeedRuleRegistry::persist() const {
//     // repo mentés – audit log is itt keletkezik
//     NeedRuleRepository::save();
// }
