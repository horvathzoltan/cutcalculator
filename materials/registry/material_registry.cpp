#include "materials/registry/material_registry.h"
#include "materials/model/material_type.h"

// QVector<MaterialMaster>
// MaterialRegistry::findByType(MaterialType type) const
// {
//     return findAll([&](const MaterialMaster& m){
//         return m.type == type;
//     });
// }

// QVector<MaterialMaster>
// MaterialRegistry::findByShape(CrossSectionShape::Shape shape) const
// {
//     return findAll([&](const MaterialMaster& m){
//         return m.shape.value == shape;
//     });
// }

// QVector<MaterialMaster>
// MaterialRegistry::findByColor(const NamedColor& color) const
// {
//     return findAll([&](const MaterialMaster& m){
//         return m.color == color;
//     });
// }

// bool MaterialRegistry::validateDomain(const MaterialMaster& m) const {
//     return !m.name.trimmed().isEmpty()
//     && m.type.value != MaterialType::Type::Unknown;
// }

// bool MaterialRegistry::validateDuplicate(const MaterialMaster& m) const {
//     return !existsBy([&](const MaterialMaster& x){
//         return x.barcode == m.barcode;
//     });
// }

