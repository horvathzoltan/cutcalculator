// products/registry/product_registry.cpp
#include "products/registry/product_registry.h"

ProductRegistry& ProductRegistry::instance() {
    static ProductRegistry inst;
    return inst;
}

void ProductRegistry::setData(const QVector<ProductMaster>& v) {
    _data = v;
}

const QVector<ProductMaster>& ProductRegistry::readAll() const {
    return _data;
}

ProductMaster* ProductRegistry::findById(const QUuid& id) {
    for (auto& def : _data) {
        if (def.id == id) return &def;
    }
    return nullptr;
}

QVector<ProductMaster> ProductRegistry::findChildren(const QUuid& parentId) const {
    QVector<ProductMaster> children;
    for (const auto& def : _data) {
        if (def.parentId == parentId) children.append(def);
    }
    return children;
}

QVector<ProductMaster> ProductRegistry::roots() const {
    QVector<ProductMaster> roots;
    for (const auto& def : _data) {
        if (def.isRoot()) roots.append(def);
    }
    return roots;
}
