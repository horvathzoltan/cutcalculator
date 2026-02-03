#include "material_registry_tester.h"

#include "materials/registry/material_registry.h"
#include "materials/repository/material_repository.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QFile>
#include <QTextStream>

void MaterialRegistryTester::writeTestCsv()
{
    QString csv = FileNameHelper::instance().getMaterialCsvFile();
    QFile::remove(csv);

    QFile f(csv);
    Q_ASSERT(f.open(QIODevice::WriteOnly | QIODevice::Text));

    QTextStream out(&f);
    out << "name,barcode,stockLength,dim1,dim2,shape,machineId,type,color,cuttingMode,paintingMode\n";
    out << "Mat1,ABC-1,6000,50,30,Rectangular,M1,Other,#ff0000,Length,Default\n";

    f.close();
}

void MaterialRegistryTester::testLoad()
{
    zInfo("→ testLoad");

    writeTestCsv();

    QVector<MaterialMaster> list;
    Q_ASSERT(MaterialRepository::load(list));
    Q_ASSERT(list.size() == 1);

    auto& reg = MaterialRegistry::instance();
    reg.clearForTest();
    reg.setAllForTest(list);

    Q_ASSERT(reg.size() == 1);

    zInfo("✓ testLoad OK");
}

void MaterialRegistryTester::testFindById()
{
    zInfo("→ testFindById");

    auto& reg = MaterialRegistry::instance();
    Q_ASSERT(reg.size() == 1);

    const MaterialMaster& m = reg.readAll().first();
    const MaterialMaster* found = reg.findById(m.id);

    Q_ASSERT(found != nullptr);
    Q_ASSERT(found->name == "Mat1");

    zInfo("✓ testFindById OK");
}

void MaterialRegistryTester::testBarcodeIndex()
{
    zInfo("→ testBarcodeIndex");

    auto& reg = MaterialRegistry::instance();

    const MaterialMaster* found = reg.findIf([](const MaterialMaster& mm){
        return mm.barcode == "ABC-1";
    });

    Q_ASSERT(found != nullptr);

    zInfo("✓ testBarcodeIndex OK");
}

bool MaterialRegistryTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    FileNameHelper::instance().setTestMode(true);

    testLoad();
    testFindById();
    testBarcodeIndex();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
