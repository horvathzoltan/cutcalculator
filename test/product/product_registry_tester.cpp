#include "product_registry_tester.h"

#include "products/registry/product_registry.h"
#include "products/repository/product_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"
#include "test/common/test_file_helper.h"
#include "common/registry/manager/registry_manager.h"

#include <QFile>
#include <QStringList>

class test_ProductRepository {
public:
    static QString toCsvLine(const ProductMaster& pm)
    {
        return ProductRepository::toCsvLine(pm);
    }
};

void ProductRegistryTester::clearCsv(const QString& path)
{
    QFile::remove(path);
}

void ProductRegistryTester::prepare()
{
    ids = TestDataBuilder::prepareStandard();
    ProductRegistry::instance().clearForTest();

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);
    clearCsv(csvPath);
}

//
// 1) INSERT
//
void ProductRegistryTester::testInsert()
{
    zInfo("→ testInsert");
    prepare();

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);

    Q_ASSERT(reg.size() == 0);

    ProductMaster p;
    p.id = QUuid::createUuid();
    p.name = "TestProduct";
    p.barcode = "TP1";
    p.parentId = QUuid();

    Q_ASSERT(reg.insert(p));

    QString expected = test_ProductRepository::toCsvLine(p);
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.contains(expected));
    zInfo("✓ testInsert OK");
}

//
// 2) UPDATE
//
void ProductRegistryTester::testUpdate()
{
    zInfo("→ testUpdate");

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);

    auto* found = reg.findIf([](const ProductMaster& pm){
        return pm.name == "TestProduct";
    });
    Q_ASSERT(found != nullptr);

    QString originalCsv = test_ProductRepository::toCsvLine(*found);

    ProductMaster updated = *found;
    updated.name = "UpdatedProduct";

    Q_ASSERT(reg.update(updated));

    QString expected = test_ProductRepository::toCsvLine(updated);
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.contains(expected));
    Q_ASSERT(!lines.contains(originalCsv));

    zInfo("✓ testUpdate OK");
}

//
// 3) DELETE
//
void ProductRegistryTester::testDelete()
{
    zInfo("→ testDelete");

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);

    auto* found = reg.findIf([](const ProductMaster& pm){
        return pm.name == "UpdatedProduct";
    });
    Q_ASSERT(found != nullptr);

    Q_ASSERT(reg.remove(found->id));

    QStringList lines = readAllLines(csvPath);
    Q_ASSERT(!lines.contains(test_ProductRepository::toCsvLine(*found)));

    zInfo("✓ testDelete OK");
}

//
// 4) CSV ROUNDTRIP
//
void ProductRegistryTester::testCsvRoundtrip()
{
    zInfo("→ testCsvRoundtrip");

    prepare();

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);

    ProductMaster p;
    p.id = QUuid::createUuid();
    p.name = "Roundtrip";
    p.barcode = "RT1";
    p.parentId = QUuid();

    // 1) Insert → CSV-be kerül
    Q_ASSERT(reg.insert(p));

    // 2) Elvárt CSV sor
    QString expected = test_ProductRepository::toCsvLine(p);

    // 3) CSV visszaolvasása
    QStringList lines = readAllLines(csvPath);

    // 4) Ellenőrzés
    Q_ASSERT(lines.contains(expected));

    zInfo("✓ testCsvRoundtrip OK");
}


//
// 5) LOAD PIPELINE
//
void ProductRegistryTester::testLoadPipeline()
{
    zInfo("→ testLoadPipeline");

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);
    clearCsv(csvPath);

    QFile f(csvPath);
    Q_ASSERT(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&f);

    out << "barcode,parentBarcode,name\n";
    out << "PB1,,P1\n";
    out << "PB2,,P2\n";

    f.close();

    QVector<ProductMaster> list;
    Q_ASSERT(ProductRepository::load(list));
    Q_ASSERT(list.size() == 2);

    auto& reg = ProductRegistry::instance();
    reg.clearForTest();
    reg.setAllForTest(list);

    Q_ASSERT(reg.size() == 2);

    zInfo("✓ testLoadPipeline OK");
}

//
// 6) REGISTRYMANAGER INTEGRATION
//
void ProductRegistryTester::testRegistryManagerLookup()
{
    zInfo("→ testRegistryManagerLookup");

    auto* reg = RegistryManager::instance().lookupRegistry<ProductRegistry>();
    Q_ASSERT(reg != nullptr);

    auto* found = reg->findIf([](const ProductMaster& pm){
        return pm.barcode == "PB1";
    });
    Q_ASSERT(found != nullptr);

    zInfo("✓ testRegistryManagerLookup OK");
}

//
// 7) POINTER STABILITY
//
void ProductRegistryTester::testPointerStability()
{
    zInfo("→ testPointerStability");

    auto& reg = ProductRegistry::instance();
    auto ptrs = reg.readAll_ptr();
    Q_ASSERT(!ptrs.isEmpty());

    const ProductMaster* p1 = ptrs[0];
    QString bc = p1->barcode;

    reg.onItemsChanged(); // index rebuild

    const ProductMaster* p2 = reg.findByBarcode(bc);
    Q_ASSERT(p2 != nullptr);
    Q_ASSERT(p1 == p2);

    zInfo("✓ testPointerStability OK");
}

//
// 8) HIERARCHY
//
void ProductRegistryTester::testHierarchy()
{
    zInfo("→ testHierarchy");

    auto& reg = ProductRegistry::instance();

    ProductMaster parent;
    parent.id = QUuid::createUuid();
    parent.name = "Parent";
    parent.barcode = "PARENT";

    ProductMaster child;
    child.id = QUuid::createUuid();
    child.name = "Child";
    child.barcode = "CHILD";
    child.parentId = parent.id;

    Q_ASSERT(reg.insert(parent));
    Q_ASSERT(reg.insert(child));

    auto* foundChild = reg.findIf([](const ProductMaster& pm){
        return pm.name == "Child";
    });
    Q_ASSERT(foundChild != nullptr);
    Q_ASSERT(foundChild->parentId == parent.id);

    zInfo("✓ testHierarchy OK");
}

//
// 9) BARCODE COLLISION
//
void ProductRegistryTester::testBarcodeCollision()
{
    zInfo("→ testBarcodeCollision");

    auto& reg = ProductRegistry::instance();

    ProductMaster p1;
    p1.id = QUuid::createUuid();
    p1.name = "BC1";
    p1.barcode = "BC";

    ProductMaster p2 = p1;
    p2.id = QUuid::createUuid();

    Q_ASSERT(reg.insert(p1));
    Q_ASSERT(!reg.insert(p2)); // collision

    zInfo("✓ testBarcodeCollision OK");
}

//
// 10) FIND FUNCTIONS
//
void ProductRegistryTester::testFindFunctions()
{
    zInfo("→ testFindFunctions");

    auto& reg = ProductRegistry::instance();

    auto* found = reg.findIf([](const ProductMaster& pm){
        return pm.name == "Parent";
    });
    Q_ASSERT(found != nullptr);

    auto all = reg.findAll([](const ProductMaster& pm){
        return pm.barcode.startsWith("P", Qt::CaseInsensitive);
    });
    Q_ASSERT(all.size() >= 2);

    zInfo("✓ testFindFunctions OK");
}

//
// 11) CSV HEADER
//
void ProductRegistryTester::testCsvHeader()
{
    zInfo("→ testCsvHeader");

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::Products, FileAccess::Write);;
    QStringList lines = readAllLines(csvPath);

    QString header = lines.first();

    Q_ASSERT(header.contains("barcode"));
    Q_ASSERT(header.contains("parentBarcode"));
    Q_ASSERT(header.contains("name"));

    zInfo("✓ testCsvHeader OK");
}

//
// RUN ALL
//
bool ProductRegistryTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    FileNameHelper::instance().setTestMode(true);

    testInsert();
    testUpdate();
    testDelete();
    testCsvRoundtrip();
    testLoadPipeline();
    testRegistryManagerLookup();
    testPointerStability();
    testHierarchy();
    testBarcodeCollision();
    testFindFunctions();
    testCsvHeader();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
