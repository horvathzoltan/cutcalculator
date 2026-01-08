#include "materials/registry/material_registry.h"
#include "common/logger/logger.h"

#include "materials/model/material_type.h"

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

bool MaterialRegistry::validateDomain(const MaterialMaster& m) const {
    return !m.name.trimmed().isEmpty()
    && m.type.value != MaterialType::Type::Unknown;
}

bool MaterialRegistry::validateDuplicate(const MaterialMaster& m) const {
    return !existsBy([&](const MaterialMaster& x){
        return x.barcode == m.barcode;   // vagy ami a domain szerint egyedi
    });
}

bool MaterialRegistry::beforeInsert(const MaterialMaster& m) {
    return true;
}

bool MaterialRegistry::beforeUpdate(const MaterialMaster& m) {
    return true;
}

void MaterialRegistry::onInsertLog(const MaterialMaster& m) {
    zInfo(QString("Material inserted: %1").arg(m.name));
}

void MaterialRegistry::onUpdateLog(const MaterialMaster& m) {
    zInfo(QString("Material updated: %1").arg(m.name));
}

void MaterialRegistry::onRemoveLog(const MaterialMaster& m) {
    zInfo(QString("Material removed: %1").arg(m.name));
}
