#pragma once
#include "common/registry/barcode/barcode_hierarchical_registry_engine.h"
#include "products/model/product_master.h"
#include "common/system/verbose_manager.h"

class ProductRegistry : public BarcodeHierarchicalRegistryEngine<ProductMaster>

{
public:   
    static ProductRegistry& instance();
    //static void initializeSingleton();

private:
    ProductRegistry();

public:
    bool registerData(const ProductMaster& e);

    // Domain-specifikus funkciók, amik maradnak:

    void persist() const;

    bool verbose() const { return IS_VERBOSE_THIS(); }
};


