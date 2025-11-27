// products/registry/product_registry.h
#pragma once
#include "products/model/product_definition.h"
#include <QVector>

/**
 * ProductRegistry
 *
 * Singleton tároló a ProductDefinition elemekhez.
 * Olvasható, kereshető, és gyerekek lekérdezése parentId alapján.
 */
class ProductRegistry {
private:
    QVector<ProductDefinition> _data;
    ProductRegistry() = default;

public:
    static ProductRegistry& instance();

    void setData(const QVector<ProductDefinition>& v);
    const QVector<ProductDefinition>& readAll() const;

    ProductDefinition* findById(const QUuid& id);
    QVector<ProductDefinition> findChildren(const QUuid& parentId) const;

    /**
     * Gyökér elemek (parentId == null).
     */
    QVector<ProductDefinition> roots() const;

    int size () const { return _data.size(); }
};
