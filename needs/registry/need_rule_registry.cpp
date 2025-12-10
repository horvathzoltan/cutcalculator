#include "needs/registry/need_rule_registry.h"
#include "needs/repository/need_rule_repository.h"

NeedRuleRegistry& NeedRuleRegistry::instance() {
    static NeedRuleRegistry inst;
    return inst;
}

QString NeedRuleRegistry::typeName() const {
    return "NeedRule";
}

int NeedRuleRegistry::size() const {
    // közvetlenül a ConnectionRegistry-ből örökölt size()
    return ConnectionRegistry<NeedRule>::size();
}

void NeedRuleRegistry::insert(const NeedRule& rule) {
    // örökölt insert → bővítve perzisztálással
    ConnectionRegistry<NeedRule>::insert(rule);
    persist();
}

bool NeedRuleRegistry::remove(const QUuid& productId, const QUuid& materialId) {
    bool ok = ConnectionRegistry<NeedRule>::remove(productId, materialId);
    if (ok) persist();
    return ok;
}

void NeedRuleRegistry::persist() const {
    // repo mentés – audit log is itt keletkezik
    NeedRuleRepository::save();
}
