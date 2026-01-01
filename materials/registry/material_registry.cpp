#include "materials/registry/material_registry.h"
#include "common/registry/barcode/barcode_registry_helper.h"

MaterialRegistry::MaterialRegistry()
    : BarcodeIdentifiableRegistryEngine<MaterialMaster>("MaterialRegistry", "MaterialMaster")
{}

MaterialRegistry& MaterialRegistry::instance() {
    static MaterialRegistry inst;
    return inst;   // <-- nincs guard
}

// void MaterialRegistry::initializeSingleton() {
//     auto& inst = instance();   // <-- ugyanaz a példány
//     inst.initialize();         // <-- explicit init
// }


bool MaterialRegistry::registerData(const MaterialMaster& e) {
    return BarcodeRegistryHelper::insert(*this, e);
}

