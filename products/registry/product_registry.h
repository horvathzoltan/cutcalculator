#pragma once
#include "common/registry/registry_engine.h"
#include "common/registry/combined_helper.h"
#include "products/model/product_master.h"

class ProductRegistry : public RegistryEngine<ProductMaster>

{
public:   
    static ProductRegistry& instance();
    static void initializeSingleton();

private:
    ProductRegistry();

public:
    // --- Hierarchia ---
    QVector<ProductMaster> findChildren(const QUuid& parentId) const {
        return CombinedHelper::findChildren(_items, parentId);
    }

    QVector<ProductMaster> roots() const {
        return CombinedHelper::roots(_items);
    }

    // --- Barcode ---
    const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const override {
        return CombinedHelper::findEntityById(_items, id);
    }

    bool registerData(const ProductMaster& e);


    // Domain-specifikus funkciók, amik maradnak:

    void persist() const;

    bool verbose() const { return IS_VERBOSE_THIS(); }
};


// products/registry/product_registry.h
// #pragma once
// #include "common/registry/barcode_identifiable_registry_base.h"
// #include "products/model/product_master.h"
// #include <QRecursiveMutex>
// #include <QVector>
//#include "common/registry/registry_base.h"

/**
 * ProductRegistry
 *
 * Singleton tároló a ProductDefinition elemekhez.
 * Olvasható, kereshető, és gyerekek lekérdezése parentId alapján.
 */
// class ProductRegistry: public BarcodeIdentifiableRegistryBase  {
// private:
//     ProductRegistry() : BarcodeIdentifiableRegistryBase("ProductRegistry", "Product") {}//    MaterialRegistry() = default;  // 🔐 Privát konstruktor a singletonhoz
//     ProductRegistry(const ProductRegistry&) = delete;

//     QVector<ProductMaster> _data;
//     mutable QRecursiveMutex _mutex;

// public:
//     // 🔁 Singleton elérés
//     static ProductRegistry& instance();

//     //QString typeName() const override { return "ProductMaster"; }
//     int size() const override { return _data.size(); }

//     void setData(const QVector<ProductMaster>& v);
//     const QVector<ProductMaster>& readAll() const;

//     const ProductMaster* findById(const QUuid& id) const;
//     ProductMaster* findByBarcode(const QString& barcode);   // 🔧 Új függvény

//     QVector<ProductMaster> findChildren(const QUuid& parentId) const;

//     /**
//      * Gyökér elemek (parentId == null).
//      */
//     QVector<ProductMaster> roots() const;

//     //int size () const { return _data.size(); }

//     // Új elem beszúrása
//     void insert(const ProductMaster& pm);

//     // Új: Update
//     bool update(const ProductMaster& updated);

//     // Elem törlése id alapján
//     bool remove(const QUuid& id);

//     // Ellenőrzi, hogy a barcode egyedi-e (kivéve a megadott id-t)
//     bool isBarcodeUnique(const QString& barcode, const QUuid& excludeId = QUuid()) const;

//     void persist() const;

//     const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const override;


// };
