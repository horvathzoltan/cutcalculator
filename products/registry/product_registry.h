#pragma once

#include <QVector>
#include <QUuid>

#include "common/registry/feature/register_me.h"

#include "common/registry/barcode/barcode_index_mixin.h"
#include "common/registry/hierarchy/hierarchy_mixin.h"

#include "common/registry/mixins/bulk_load_mixin.h"
#include "common/registry/mixins/crud_workflow_mixin.h"
#include "common/registry/mixins/test_support_mixin.h"
#include "products/model/product_master.h"
#include "common/registry/base/registry_engine_base.h"

// ⚠️ CRUD WORKFLOW – VÉGLEGES SZERZŐDÉS (WF-4)
//
// A domain CRUD egyetlen érvényes útvonala (insert/update/remove):
//   validateDomain
//   → validateDuplicate
//   → beforeInsert / beforeUpdate / beforeRemove
//   → store*Impl + notifyItemsChanged
//   → on*Log
//   → persist()
//   → after*Hook (opcionális, mellékhatás-mentes)
//
// Kötelező invariánsok:
// - nincs engine-level write API hívás (add/updateInternal/removeInternal/clear)
// - nincs kézi persist() hívás workflow-on kívül
// - nincs közvetlen memória-módosítás store*Impl megkerülésével
// - nincs CRUD hívás load-path alatt (setAll, konstruktor, CSV betöltés)
// - nincs CRUD hívás konstruktorból/destruktorból
// - nincs aszinkron vagy deferred CRUD
//
// A workflow lépései megszakíthatatlanok, sorrendkötöttek, és nem hagyhatnak
// részleges állapotot. A log mindig a persist ELŐTT fut.

class ProductRegistry
    : public RegistryEngineBase<ProductMaster>,
      public CrudWorkflowMixin<ProductRegistry, ProductMaster>,
      public BarcodeIndexMixin<ProductRegistry, ProductMaster>,
      public HierarchyMixin<ProductRegistry, ProductMaster>,
      public RegisterMe<ProductRegistry>,
      public TestSupportMixin<ProductRegistry>,
      public BulkLoadMixin<ProductRegistry, ProductMaster>
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

    bool validateDomain(const ProductMaster& p) const;
    bool validateDuplicate(const ProductMaster& p) const;

    bool beforeRemove(ProductMaster&) { return true; }
    bool beforeInsert(ProductMaster& p);
    bool beforeUpdate(ProductMaster& p);

    void afterInsert(const ProductMaster&) {}
    void afterUpdate(const ProductMaster&) {}
    void afterRemove(const ProductMaster&) {}

    void onInsertLog(const ProductMaster& p);
    void onUpdateLog(const ProductMaster& p);
    void onRemoveLog(const ProductMaster& p);

    void onItemsChanged() override {
        zInfo("róka4: ProductRegistry::onItemsChanged fut");
        RegistryEngineBase<ProductMaster>::onItemsChanged();
    }

private:
    ProductRegistry()
        : RegistryEngineBase("ProductRegistry", "ProductMaster")
    {}
};



