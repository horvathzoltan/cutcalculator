#pragma once

#include "common/registry/base/registry_engine_base.h"
#include "common/registry/mixins/crud_workflow_mixin.h"
#include "common/registry/mixins/bulk_load_mixin.h"
#include "common/registry/mixins/simple_lookup_mixin.h"
#include "common/registry/mixins/test_support_mixin.h"
#include "common/registry/feature/register_me.h"

#include "orders/model/order_item.h"
#include "common/logger/logger.h"

class OrderItemRegistry
    : public RegistryEngineBase<OrderItem>,
      public CrudWorkflowMixin<OrderItemRegistry, OrderItem>,
      public BulkLoadMixin<OrderItemRegistry, OrderItem>,
      public TestSupportMixin<OrderItemRegistry>,
      //public SimpleLookupMixin<OrderItemRegistry, OrderItem>,
      public RegisterMe<OrderItemRegistry>
{
    AUTO_REGISTER_REGISTRY

    REGISTRY_CTOR(OrderItemRegistry, OrderItem)

    // private:
    //     OrderItemRegistry()
    //         : RegistryEngineBase("OrderItemRegistry", "OrderItem")
    //         {}

    public:
        static OrderItemRegistry& instance() {
            static OrderItemRegistry inst;
            inst.initialize();
            return inst;
        }

    bool insert(const OrderItem& it)  { return insertWithWorkflow(it); }
    bool update(const OrderItem& it)  { return updateWithWorkflow(it); }
    bool remove(const QUuid& id)      { return removeWithWorkflow(id); }

    // Domain validation
    bool validateDomain(const OrderItem& it) const;
    bool validateDuplicate(const OrderItem& it) const;

    // Workflow hooks
    bool beforeInsert(OrderItem& it);
    bool beforeUpdate(OrderItem& it);
    bool beforeRemove(OrderItem&) { return true; }

    void afterInsert(const OrderItem&) {}
    void afterUpdate(const OrderItem&) {}
    void afterRemove(const OrderItem&) {}

    void onInsertLog(const OrderItem& it);
    void onUpdateLog(const OrderItem& it);
    void onRemoveLog(const OrderItem& it);

    void persist() const;

    QVector<OrderItem> findByOrderId(const QUuid& orderId) const {
        return findAll([&](const OrderItem& it){ return it.orderId == orderId; });
    }
};
