#include "product_registry.h"
#include "products/repository/product_repository.h"
#include "common/utils/filename_helper.h"
#include "common/registry/barcode/barcode_registry_helper.h"

ProductRegistry::ProductRegistry()
    : BarcodeHierarchicalRegistryEngine<ProductMaster>("ProductRegistry", "Product")
{}

ProductRegistry& ProductRegistry::instance() {
    static ProductRegistry inst;
    return inst; // NINCS guard
}

// void ProductRegistry::initializeSingleton() {
//     auto& inst = instance();
//     inst.initialize();
// }

bool ProductRegistry::registerData(const ProductMaster& e) {
    return BarcodeRegistryHelper::insert(*this, e);
}


void ProductRegistry::persist() const {
    const QString path = FileNameHelper::instance().getProductCsvFile();
    if (path.isEmpty()) return;

    ProductRepository::saveToCSV(_items, path);
}
