// products/registry/product_registry.h
#pragma once
#include "products/model/product_master.h"
#include <QVector>
#include "common/registry/registry_base.h"

/**
 * ProductRegistry
 *
 * Singleton tároló a ProductDefinition elemekhez.
 * Olvasható, kereshető, és gyerekek lekérdezése parentId alapján.
 */
class ProductRegistry: public RegistryBase  {
private:
    ProductRegistry() : RegistryBase("ProductRegistry") {}//    MaterialRegistry() = default;  // 🔐 Privát konstruktor a singletonhoz
    ProductRegistry(const ProductRegistry&) = delete;

    QVector<ProductMaster> _data;
public:
    // 🔁 Singleton elérés
    static ProductRegistry& instance();

    QString typeName() const override { return "ProductMaster"; }
    int size() const override { return _data.size(); }

    void setData(const QVector<ProductMaster>& v);
    const QVector<ProductMaster>& readAll() const;

    ProductMaster* findById(const QUuid& id);
    QVector<ProductMaster> findChildren(const QUuid& parentId) const;

    /**
     * Gyökér elemek (parentId == null).
     */
    QVector<ProductMaster> roots() const;

    //int size () const { return _data.size(); }
};
