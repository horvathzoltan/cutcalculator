#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/base/registry_engine.h"
#include "common/registry/workflow/connection_workflow_policy.h"

#include "needs/model/need_rule.h"
//#include "needs/repository/need_rule_repository.h"
//#include "common/logger/logger.h"

class NeedRuleRegistry
    : public RegistryEngine<NeedRule, ConnectionWorkflowPolicy>,
    public RegisterMe<NeedRuleRegistry>
{
    AUTO_REGISTER_REGISTRY;
public:
    static NeedRuleRegistry& instance() {
        static NeedRuleRegistry inst;
        return inst;
    }

    QVector<NeedRule> findByLeft(const QUuid& leftId) const;

    bool remove(const QUuid& leftId, const QUuid& rightId) {
        return removeInternal(leftId, rightId);
    }

protected:
    bool validateConnection(const NeedRule& r) const override;
    bool validateDuplicate(const NeedRule& r) const override;

    bool beforeInsert(const NeedRule& r) override;
    void afterInsert(const NeedRule& r) override;

    void onInsertLog(const NeedRule& r) override;
    void onRemoveLog(const NeedRule& r) override;

    void persist() const override;

private:
    NeedRuleRegistry(); // unchanged
};


// #pragma once

// #include "connections/connection_registry_engine.h"
// #include "needs/model/need_rule.h"
// #include "common/registry/feature/register_me.h"

// class NeedRuleRegistry : public ConnectionRegistryEngine<NeedRule>,
//                          public RegisterMe<NeedRuleRegistry>
// {
//     AUTO_REGISTER_REGISTRY(NeedRuleRegistry);
// public:
//     static NeedRuleRegistry& instance();

// private:
//     NeedRuleRegistry();

//     NeedRuleRegistry(const NeedRuleRegistry&) = delete;
//     NeedRuleRegistry& operator=(const NeedRuleRegistry&) = delete;

// public:
//     void insert(const NeedRule& rule);
//     bool remove(const QUuid& productId, const QUuid& materialId);

//     void persist() const;
// };


// #pragma once
// #include "connections/connection_registry.h"
// #include "needs/model/need_rule.h"
// #include "common/registry/base/registry_base.h"

// /**
//  * 📚 NeedRuleRegistry – auditbarát, on-the-fly perzisztáló kapcsolattábla
//  *
//  * Örököl a IdentifiableRegistryBase-ből (audit) és a ConnectionRegistry<NeedRule>-ből (CRUD).
//  */
// class NeedRuleRegistry
//     : public RegistryBase,
//       public ConnectionRegistry<NeedRule> {

// private:
//     NeedRuleRegistry() : RegistryBase("NeedRuleRegistry", "NeedRule") {}
//     NeedRuleRegistry(const NeedRuleRegistry&) = delete;

// public:
//     static NeedRuleRegistry& instance();

//     //QString typeName() const override;
//     int size() const override;

//     void insert(const NeedRule& rule);
//     bool remove(const QUuid& productId, const QUuid& materialId);

//     void persist() const;
// };
