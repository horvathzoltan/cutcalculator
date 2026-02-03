#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/mixins/connection_workflow_mixin.h"
#include "common/registry/base/registry_engine_base.h"
#include "common/registry/mixins/connection_crud_mixin.h"
#include "needs/model/need_rule.h"
//#include "needs/repository/need_rule_repository.h"
//#include "common/logger/logger.h"

class NeedRuleRegistry
    : public RegistryEngineBase<NeedRule>,
      public ConnectionCrudMixin<NeedRuleRegistry, NeedRule>,
      public ConnectionWorkflowMixin<NeedRuleRegistry, NeedRule>,
      public RegisterMe<NeedRuleRegistry>,
      public TestSupportMixin<NeedRuleRegistry>
{
    AUTO_REGISTER_REGISTRY;
public:
    static NeedRuleRegistry& instance() {
        static NeedRuleRegistry inst;
        return inst;
    }

    QVector<NeedRule> findByLeft(const QUuid& leftId) const;

        // Publikus API – NEM írjuk felül az insert/remove-ot!
    bool insertRule(const QUuid& leftId, const QUuid& rightId) {
        return insertWithWorkflow(leftId, rightId);
    }

    bool removeRule(const QUuid& leftId, const QUuid& rightId) {
        return removeWithWorkflow(leftId, rightId);
    }

    // Domain hookok
    bool validateConnection(const NeedRule& r) const;
    bool validateDuplicate(const NeedRule& r) const;

    bool beforeInsert(NeedRule&) { return true; }
    //void afterInsert(const NeedRule& r);

    bool beforeRemove(NeedRule&) { return true; }

    void onInsertLog(const NeedRule& r);
    void onRemoveLog(const NeedRule& r);

    void persist() const;


private:
    NeedRuleRegistry(); // unchanged
};


