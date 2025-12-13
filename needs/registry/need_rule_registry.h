#pragma once
#include "connections/connection_registry.h"
#include "needs/model/need_rule.h"
#include "common/registry/registry_base.h"

/**
 * 📚 NeedRuleRegistry – auditbarát, on-the-fly perzisztáló kapcsolattábla
 *
 * Örököl a IdentifiableRegistryBase-ből (audit) és a ConnectionRegistry<NeedRule>-ből (CRUD).
 */
class NeedRuleRegistry
    : public RegistryBase,
      public ConnectionRegistry<NeedRule> {

private:
    NeedRuleRegistry() : RegistryBase("NeedRuleRegistry", "NeedRule") {}
    NeedRuleRegistry(const NeedRuleRegistry&) = delete;

public:
    static NeedRuleRegistry& instance();

    //QString typeName() const override;
    int size() const override;

    void insert(const NeedRule& rule);
    bool remove(const QUuid& productId, const QUuid& materialId);

    void persist() const;
};
