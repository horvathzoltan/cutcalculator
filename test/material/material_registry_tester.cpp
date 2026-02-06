#include "material_registry_tester.h"

#include "materials/registry/material_registry.h"
#include "materials/repository/material_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QFile>
#include <QTextStream>

void MaterialRegistryTester::writeTestCsv_multi()
{
    QString csv = FileNameHelper::instance().getMaterialCsvFile();
    QFile::remove(csv);

    QFile f(csv);
    Q_ASSERT(f.open(QIODevice::WriteOnly | QIODevice::Text));

    QTextStream out(&f);
    out << "name,barcode,stockLength,dim1,dim2,shape,machineId,type,color,cuttingMode,paintingMode\n";

    out << "Mat1,ABC-1,6000,50,30,Rectangular,M1,Custom,RAL 3020,Length,Default\n";
    out << "Mat2,ABC-2,6000,40,20,Rectangular,M1,Custom,RAL 6018,Length,Default\n";
    out << "Mat3,XYZ-1,6000,30,10,Rectangular,M1,Custom,RAL 5015,Length,Default\n";
    out << "Mat4,abc-3,6000,20,10,Rectangular,M1,Custom,RAL 7040,Length,Default\n";
    out << "Mat5,ABC-1-EXTRA,6000,10,5,Rectangular,M1,Custom,RAL 9006,Length,Default\n";

    f.close();
}

void MaterialRegistryTester::testLoad_multi()
{
    zInfo("→ testLoad_multi");

    writeTestCsv_multi();

    QVector<MaterialMaster> list;
    Q_ASSERT(MaterialRepository::load(list));
    Q_ASSERT(list.size() == 5);

    auto& reg = MaterialRegistry::instance();
    reg.clearForTest();
    reg.setAllForTest(list);

    Q_ASSERT(reg.size() == 5);

    zInfo("✓ testLoad_multi OK");
}

void MaterialRegistryTester::testFindByBarcode()
{
    zInfo("→ testFindByBarcode");

    auto& reg = MaterialRegistry::instance();

    const MaterialMaster* m1 = reg.findByBarcode("ABC-1");
    Q_ASSERT(m1 != nullptr);
    Q_ASSERT(m1->name == "Mat1");

    const MaterialMaster* m4 = reg.findByBarcode("abc-3"); // lowercase
    Q_ASSERT(m4 != nullptr);
    Q_ASSERT(m4->name == "Mat4");

    const MaterialMaster* m1_case = reg.findByBarcode("aBc-1");
    Q_ASSERT(m1_case != nullptr);
    Q_ASSERT(m1_case->name == "Mat1");

    zInfo("✓ testFindByBarcode OK");
}

void MaterialRegistryTester::testFindIfMultiple()
{
    zInfo("→ testFindIfMultiple");

    auto& reg = MaterialRegistry::instance();

    auto all = reg.RegistryEngineBase<MaterialMaster>::findAll(
        [](const MaterialMaster& mm){
            return mm.barcode.startsWith("ABC", Qt::CaseInsensitive);
        }
        );

    Q_ASSERT(all.size() == 4);

    zInfo("✓ testFindIfMultiple OK");
}

void MaterialRegistryTester::testPointerStability()
{
    zInfo("→ testPointerStability");

    auto& reg = MaterialRegistry::instance();

    const auto ptrs = reg.readAll_ptr();
    Q_ASSERT(ptrs.size() == 5);

    const MaterialMaster* p1 = ptrs[0];
    QString oldBarcode = p1->barcode;

    reg.onItemsChanged(); // index rebuild

    const MaterialMaster* p2 = reg.findByBarcode(oldBarcode);
    Q_ASSERT(p2 != nullptr);

    Q_ASSERT(p1 == p2); // pointer stability

    zInfo("✓ testPointerStability OK");
}

bool MaterialRegistryTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    FileNameHelper::instance().setTestMode(true);

    testLoad_multi();
    testFindByBarcode();
    testFindIfMultiple();
    testPointerStability();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
