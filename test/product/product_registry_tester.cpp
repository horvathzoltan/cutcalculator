#include "product_registry_tester.h"

#include "products/registry/product_registry.h"
#include "products/repository/product_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"
#include "test/common/test_file_helper.h"

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
    ProductRegistry::instance().clear();

    QString csvPath = FileNameHelper::instance().getProductCsvFile();
    clearCsv(csvPath);
}

void ProductRegistryTester::testInsert()
{
    zInfo("→ testInsert");
    prepare();

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().getProductCsvFile();

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

void ProductRegistryTester::testUpdate()
{
    zInfo("→ testUpdate");

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().getProductCsvFile();

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

void ProductRegistryTester::testDelete()
{
    zInfo("→ testDelete");

    auto& reg = ProductRegistry::instance();
    QString csvPath = FileNameHelper::instance().getProductCsvFile();

    auto* found = reg.findIf([](const ProductMaster& pm){
        return pm.name == "UpdatedProduct";
    });
    Q_ASSERT(found != nullptr);

    Q_ASSERT(reg.remove(found->id));

    QStringList lines = readAllLines(csvPath);
    Q_ASSERT(!lines.contains(test_ProductRepository::toCsvLine(*found)));

    zInfo("✓ testDelete OK");
}

bool ProductRegistryTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    FileNameHelper::instance().setTestMode(true);

    testInsert();
    testUpdate();
    testDelete();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
