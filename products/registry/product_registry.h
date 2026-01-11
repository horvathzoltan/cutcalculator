#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/feature/register_me.h"

#include "common/registry/barcode/barcode_index_mixin.h"
#include "common/registry/hierarchy/hierarchy_mixin.h"

#include "common/registry/mixins/crud_workflow_mixin.h"
#include "products/model/product_master.h"
#include "common/registry/mixins/crud_mixin.h"
#include "common/registry/base/registry_engine_base.h"

class ProductRegistry
    : public RegistryEngineBase<ProductMaster>,
      public CrudMixin<ProductRegistry, ProductMaster>,
      public CrudWorkflowMixin<ProductRegistry, ProductMaster>,
      public BarcodeIndexMixin<ProductRegistry, ProductMaster>,
      public HierarchyMixin<ProductRegistry, ProductMaster>,
      public RegisterMe<ProductRegistry>
{
    AUTO_REGISTER_REGISTRY;
public:
    static ProductRegistry& instance() {
        static ProductRegistry inst;
        inst.initialize();
        return inst;
    }

    void persist() const;
    bool insert(const ProductMaster& p) { return insertWithWorkflow(p); }
    bool update(const ProductMaster& p) { return updateWithWorkflow(p); }
    bool remove(const QUuid& id) { return removeWithWorkflow(id); }

    // Domain hookok
    bool validateDomain(const ProductMaster& p) const;
    bool validateDuplicate(const ProductMaster& p) const;

    bool beforeRemove(ProductMaster&) { return true; }

    bool beforeInsert(ProductMaster& p);
    bool beforeUpdate(ProductMaster& p);

    void onInsertLog(const ProductMaster& p);
    void onUpdateLog(const ProductMaster& p);
    void onRemoveLog(const ProductMaster& p);

private:
    ProductRegistry()
        : RegistryEngineBase("ProductRegistry", "ProductMaster")
    {}
};

// #pragma once
// #include "common/registry/barcode/barcode_hierarchical_registry_engine.h"
// #include "products/model/product_master.h"
// #include "common/system/verbose_manager.h"
// #include "common/registry/feature/register_me.h"

// class ProductRegistry : public BarcodeHierarchicalRegistryEngine<ProductMaster>,
//                         public RegisterMe<ProductRegistry>

// {
//     AUTO_REGISTER_REGISTRY(ProductRegistry);
// private:
//     ProductRegistry();
// public:
//     static ProductRegistry& instance() {
//         static ProductRegistry inst;
//         return inst; // NINCS guard
//     }

//     bool insert(const ProductMaster& e);
//     bool remove(const QUuid& id);
//     bool update(const ProductMaster& e);

//     void persist() const override;

//     bool verbose() const { return IS_VERBOSE_THIS(); }
// };


