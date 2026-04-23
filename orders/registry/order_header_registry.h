#pragma once

#include "common/registry/base/registry_engine_base.h"
#include "common/registry/mixins/crud_workflow_mixin.h"
#include "common/registry/mixins/bulk_load_mixin.h"
#include "common/registry/mixins/test_support_mixin.h"
#include "common/registry/feature/register_me.h"

#include "orders/model/order_header.h"
#include "common/logger/logger.h"

class OrderHeaderRegistry
    : public RegistryEngineBase<OrderHeader>,
      public CrudWorkflowMixin<OrderHeaderRegistry, OrderHeader>,
      public BulkLoadMixin<OrderHeaderRegistry, OrderHeader>,
      public TestSupportMixin<OrderHeaderRegistry>,
      public RegisterMe<OrderHeaderRegistry>
{
    AUTO_REGISTER_REGISTRY

    REGISTRY_CTOR(OrderHeaderRegistry, OrderHeader)
    // private:
    // OrderHeaderRegistry()
    //     : RegistryEngineBase("OrderHeaderRegistry", "OrderHeader")
    //     {}

    public:
        static OrderHeaderRegistry& instance() {
            static OrderHeaderRegistry inst;
            inst.initialize();
            return inst;
        }

    bool insert(const OrderHeader& h)  { return insertWithWorkflow(h); }
    bool update(const OrderHeader& h)  { return updateWithWorkflow(h); }
    bool remove(const QUuid& id)       { return removeWithWorkflow(id); }

    // Domain validation
    bool validateDomain(const OrderHeader& h) const;
    bool validateDuplicate(const OrderHeader& h) const;

    // Workflow hooks
    bool beforeInsert(OrderHeader& h);
    bool beforeUpdate(OrderHeader& h);
    bool beforeRemove(OrderHeader&) { return true; }

    void afterInsert(const OrderHeader&) {}
    void afterUpdate(const OrderHeader&) {}
    void afterRemove(const OrderHeader&) {}

    void onInsertLog(const OrderHeader& h);
    void onUpdateLog(const OrderHeader& h);
    void onRemoveLog(const OrderHeader& h);

    void persist() const;
};
