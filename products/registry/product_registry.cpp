// products/registry/product_registry.cpp
#include "products/registry/product_registry.h"

ProductRegistry& ProductRegistry::instance() {
    static ProductRegistry inst;
    return inst;
}

void ProductRegistry::setData(const QVector<ProductDefinition>& v) {
    _data = v;
}

const QVector<ProductDefinition>& ProductRegistry::readAll() const {
    return _data;
}

ProductDefinition* ProductRegistry::findById(const QUuid& id) {
    for (auto& def : _data) {
        if (def.id == id) return &def;
    }
    return nullptr;
}

QVector<ProductDefinition> ProductRegistry::findChildren(const QUuid& parentId) const {
    QVector<ProductDefinition> children;
    for (const auto& def : _data) {
        if (def.parentId == parentId) children.append(def);
    }
    return children;
}

QVector<ProductDefinition> ProductRegistry::roots() const {
    QVector<ProductDefinition> roots;
    for (const auto& def : _data) {
        if (def.isRoot()) roots.append(def);
    }
    return roots;
}
