#include "product_registry.h"
#include "products/repository/product_repository.h"
#include "common/utils/filename_helper.h"
#include "common/registry/barcode/barcode_registry_helper.h"

ProductRegistry::ProductRegistry()
    : BarcodeHierarchicalRegistryEngine<ProductMaster>("ProductRegistry", "Product")
{}


bool ProductRegistry::insert(const ProductMaster& e) {
    return BarcodeRegistryHelper::insert(*this, e);
}

bool ProductRegistry::remove(const QUuid& id) {
    return IdentifiableRegistryHelper::remove(*this, id);
}

bool ProductRegistry::update(const ProductMaster& e) {
    return IdentifiableRegistryHelper::update(*this, e);
}

void ProductRegistry::persist() const {
    const QString path = FileNameHelper::instance().getProductCsvFile();
    if (path.isEmpty()) return;

    ProductRepository::saveToCSV(_items, path);
}
