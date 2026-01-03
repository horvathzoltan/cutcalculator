#pragma once
#include "common/registry/barcode/barcode_hierarchical_registry_engine.h"
#include "products/model/product_master.h"
#include "common/system/verbose_manager.h"
#include "common/registry/feature/register_me.h"

class ProductRegistry : public BarcodeHierarchicalRegistryEngine<ProductMaster>,
                        public RegisterMe<ProductRegistry>

{
    AUTO_REGISTER_REGISTRY(ProductRegistry);
private:
    ProductRegistry();
public:
    static ProductRegistry& instance() {
        static ProductRegistry inst;
        return inst; // NINCS guard
    }

    bool insert(const ProductMaster& e);
    bool remove(const QUuid& id);
    bool update(const ProductMaster& e);

    void persist() const override;

    bool verbose() const { return IS_VERBOSE_THIS(); }
};


