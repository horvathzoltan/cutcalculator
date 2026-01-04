#include "materials/registry/material_registry.h"

// --- Lookup API ---

QVector<MaterialMaster>
MaterialRegistry::findByType(MaterialType type) const
{
    return findAll([&](const MaterialMaster& m){
        return m.type == type;
    });
}

QVector<MaterialMaster>
MaterialRegistry::findByShape(CrossSectionShape::Shape shape) const
{
    return findAll([&](const MaterialMaster& m){
        return m.shape.value == shape;
    });
}

QVector<MaterialMaster>
MaterialRegistry::findByColor(const NamedColor& color) const
{
    return findAll([&](const MaterialMaster& m){
        return m.color == color;   // működik, ha NamedColor kap operator==‑t
    });
}


// #include "materials/registry/material_registry.h"

// #include "common/registry/barcode/barcode_registry_helper.h"

// MaterialRegistry::MaterialRegistry()
//     : BarcodeIdentifiableRegistryEngine<MaterialMaster>("MaterialRegistry", "MaterialMaster")
// {}

// bool MaterialRegistry::insert(const MaterialMaster& e) {
//     return BarcodeRegistryHelper::insert(*this, e);
// }

