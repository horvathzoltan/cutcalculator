#include "product_registry_tester.h"

#include "products/registry/product_registry.h"
#include "products/repository/product_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QFile>
#include <QStringList>

// --- Helpers -------------------------------------------------------------

class test_ProductRepository {
public:
    static QString toCsvLine(const ProductMaster& pm)
    {
        return ProductRepository::toCsvLine(pm);
    }
};

// static void clearRegistry(ProductRegistry& reg)
// {
//     reg.clear();
// }

static void clearCsv(const QString& path)
{
    QFile::remove(path);
}

static QStringList readAllLines(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QStringList lines;
    while (!f.atEnd())
        lines << QString::fromUtf8(f.readLine()).trimmed();

    return lines;
}

// --- CRUD TESTS ----------------------------------------------------------

static void testInsert(ProductRegistry& reg, const QString& csvPath)
{
    zInfo("→ testInsert");

    // 1) Üres registry + üres CSV
    Q_ASSERT(reg.size() == 0);

    // 2) Létrehozunk egy új ProductMaster-t
    ProductMaster p;
    p.id = QUuid::createUuid();
    p.name = "TestProduct";
    p.barcode = "TP1";
    p.parentId = QUuid(); // root

    Q_ASSERT(reg.insert(p));   // persist() automatikusan fut

    // 3) CSV ellenőrzés
    QString expected = test_ProductRepository::toCsvLine(p);
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.contains(expected));
    zInfo("✓ testInsert OK");
}


static void testUpdate(ProductRegistry& reg, const QString& csvPath)
{
    zInfo("→ testUpdate");

    auto* found = reg.findIf([](const ProductMaster& pm){
        return pm.name == "TestProduct";
    });
    Q_ASSERT(found != nullptr);

    // 1) Eredeti CSV sor elmentése
    QString originalCsv = test_ProductRepository::toCsvLine(*found);

    // 2) Módosítás
    ProductMaster updated = *found;
    updated.name = "UpdatedProduct";

    Q_ASSERT(reg.update(updated));

    // 3) CSV ellenőrzés
    QString expected = test_ProductRepository::toCsvLine(updated);
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.contains(expected));
    Q_ASSERT(!lines.contains(originalCsv));   // <-- EZ A HELYES
}



static void testDelete(ProductRegistry& reg, const QString& csvPath)
{
    zInfo("→ testDelete");

    // 1) Megkeressük a frissített elemet
    auto* found = reg.findIf([](const ProductMaster& pm){
        return pm.name == "UpdatedProduct";
    });
    Q_ASSERT(found != nullptr);

    // 2) Töröljük
    Q_ASSERT(reg.remove(found->id));   // persist() automatikusan fut

    // 3) CSV ellenőrzés
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(!lines.contains(test_ProductRepository::toCsvLine(*found)));

    zInfo("✓ testDelete OK");
}

// --- Test entry point ----------------------------------------------------

bool runProductRegistryTests()
{
    zInfo("=== ProductRegistry CRUD TESTS START ===");

    auto& reg = ProductRegistry::instance();
    FileNameHelper::instance().setTestMode(true);

    const QString csvPath = FileNameHelper::instance().getProductCsvFile();

    reg.clear();
    clearCsv(csvPath);

    testInsert(reg, csvPath);
    testUpdate(reg, csvPath);
    testDelete(reg, csvPath);

    zInfo("=== ProductRegistry CRUD TESTS END ===");
    return true;
}
