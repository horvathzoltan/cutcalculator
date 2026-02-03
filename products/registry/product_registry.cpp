#include "products/registry/product_registry.h"
#include "products/repository/product_repository.h"
#include "common/utils/filename_helper.h"

// --- Lookup API ---

// QVector<ProductMaster>
// ProductRegistry::findChildren(const QUuid& parentId) const
// {
//     return findAll([&](const ProductMaster& p){
//         return p.parentId == parentId;
//     });
// }

// QVector<ProductMaster>
// ProductRegistry::findRoots() const
// {
//     return findAll([&](const ProductMaster& p){
//         return p.parentId.isNull();
//     });
// }

// --- Domain hookok ---

bool ProductRegistry::validateDomain(const ProductMaster& p) const
{
    return !p.name.isEmpty()
    && !p.barcode.isEmpty();
}

bool ProductRegistry::validateDuplicate(const ProductMaster& p) const
{
    return !existsBy([&](const ProductMaster& x){
        return x.name == p.name
               && x.parentId == p.parentId
               && x.id != p.id;
    });

}

bool ProductRegistry::beforeInsert( ProductMaster&)
{
    return true;
}

bool ProductRegistry::beforeUpdate(ProductMaster&)
{
    return true;
}

// --- Log hookok ---

void ProductRegistry::onInsertLog(const ProductMaster& p)
{
    zInfo(QString("➕ Product INSERT: %1").arg(p.name));
}

void ProductRegistry::onUpdateLog(const ProductMaster& p)
{
    zInfo(QString("✏️ Product UPDATE: %1").arg(p.name));
}

void ProductRegistry::onRemoveLog(const ProductMaster& p)
{
    zInfo(QString("🗑️ Product REMOVE: %1").arg(p.name));
}

// --- Persist ---

void ProductRegistry::persist() const
{
    const QString path = FileNameHelper::instance().getProductCsvFile();
    if (path.isEmpty())
        return;

    ProductRepository::saveToCSV(readAll(), path);
}

