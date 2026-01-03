#include "materials/registry/material_registry.h"
#include "common/registry/barcode/barcode_registry_helper.h"

MaterialRegistry::MaterialRegistry()
    : BarcodeIdentifiableRegistryEngine<MaterialMaster>("MaterialRegistry", "MaterialMaster")
{}

bool MaterialRegistry::insert(const MaterialMaster& e) {
    return BarcodeRegistryHelper::insert(*this, e);
}

