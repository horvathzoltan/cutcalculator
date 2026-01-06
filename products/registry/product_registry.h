#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/base/registry_engine.h"
#include "common/registry/feature/register_me.h"
#include "common/registry/workflow/crud_workflow_policy.h"
//#include "common/registry/barcode/barcode_lookup_mixin.h"
#include "common/registry/barcode/barcode_index_mixin.h"
#include "common/registry/hierarchy/hierarchy_mixin.h"

#include "products/model/product_master.h"
//#include "common/logger/logger.h"

class ProductRegistry
    : public RegistryEngine<ProductMaster, CrudWorkflowPolicy>,
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

    // Hierarchia lookupok
    // QVector<ProductMaster> findChildren(const QUuid& parentId) const;
    // QVector<ProductMaster> findRoots() const;

    void persist() const override;

protected:
    // Domain hookok
    bool validateDomain(const ProductMaster& p) const override;
    bool validateDuplicate(const ProductMaster& p) const override;

    bool beforeInsert(const ProductMaster& p) override;
    bool beforeUpdate(const ProductMaster& p) override;

    void onInsertLog(const ProductMaster& p) override;
    void onUpdateLog(const ProductMaster& p) override;
    void onRemoveLog(const ProductMaster& p) override;

    void onLoadLog() override;

private:
    ProductRegistry()
        : RegistryEngine("ProductRegistry", "ProductMaster")
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


