#include "need_calculation_registry_tester.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/model/need_calculation.h"
#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"
#include "test/common/test_data_builder.h"
#include "test/common/test_file_helper.h"
#include "calcmodes/repository/need_calculation_repository.h"

#include <QFile>
#include <QStringList>

NeedCalculation NeedCalculationRegistryTester::makeNC(const QUuid& productId,
                                                      const QString& name)
{
    NeedCalculation nc;
    nc.id = QUuid::createUuid();
    nc.productId = productId;
    nc.name = name;
    return nc;
}

void NeedCalculationRegistryTester::prepare()
{
    ids = TestDataBuilder::prepareStandard();
    NeedCalculationRegistry::instance().clear();

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QFile::remove(csvPath);
}

void NeedCalculationRegistryTester::testValidInsert()
{
    zInfo("→ testValidInsert");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();
    NeedCalculation nc = makeNC(ids.P1, "ModeA");

    Q_ASSERT(reg.insert(nc));
    Q_ASSERT(reg.size() == 1);

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    QString expected = NeedCalculationRepository::toCsvLine(nc);
    Q_ASSERT(lines.contains(expected));

    zInfo("✓ testValidInsert OK");
}

void NeedCalculationRegistryTester::testDuplicateInsert()
{
    zInfo("→ testDuplicateInsert");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc = makeNC(ids.P1, "ModeA");
    Q_ASSERT(reg.insert(nc));

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList before = readAllLines(csvPath);

    NeedCalculation dup = makeNC(ids.P1, "ModeA");
    Q_ASSERT(!reg.insert(dup));

    QStringList after = readAllLines(csvPath);
    Q_ASSERT(before == after);

    zInfo("✓ testDuplicateInsert OK");
}

void NeedCalculationRegistryTester::testInvalidDomain()
{
    zInfo("→ testInvalidDomain");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation bad = makeNC(ids.P1, "");
    Q_ASSERT(!reg.insert(bad));

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.isEmpty());

    zInfo("✓ testInvalidDomain OK");
}

void NeedCalculationRegistryTester::testValidUpdate()
{
    zInfo("→ testValidUpdate");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc = makeNC(ids.P1, "ModeA");
    Q_ASSERT(reg.insert(nc));

    nc.name = "ModeA2";
    Q_ASSERT(reg.update(nc));

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    QString expected = NeedCalculationRepository::toCsvLine(nc);
    Q_ASSERT(lines.contains(expected));

    zInfo("✓ testValidUpdate OK");
}

void NeedCalculationRegistryTester::testInvalidUpdateDuplicate()
{
    zInfo("→ testInvalidUpdateDuplicate");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc1 = makeNC(ids.P1, "ModeA");
    NeedCalculation nc2 = makeNC(ids.P1, "ModeB");

    Q_ASSERT(reg.insert(nc1));
    Q_ASSERT(reg.insert(nc2));

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList before = readAllLines(csvPath);

    nc2.name = "ModeA";
    Q_ASSERT(!reg.update(nc2));

    QStringList after = readAllLines(csvPath);
    Q_ASSERT(before == after);

    zInfo("✓ testInvalidUpdateDuplicate OK");
}

void NeedCalculationRegistryTester::testValidRemove()
{
    zInfo("→ testValidRemove");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc = makeNC(ids.P1, "ModeA");
    Q_ASSERT(reg.insert(nc));

    Q_ASSERT(reg.remove(nc.id));
    Q_ASSERT(reg.size() == 0);

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    QString removed = NeedCalculationRepository::toCsvLine(nc);
    Q_ASSERT(!lines.contains(removed));

    zInfo("✓ testValidRemove OK");
}

void NeedCalculationRegistryTester::testInvalidRemove()
{
    zInfo("→ testInvalidRemove");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList before = readAllLines(csvPath);

    Q_ASSERT(!reg.remove(QUuid::createUuid()));

    QStringList after = readAllLines(csvPath);
    Q_ASSERT(before == after);

    zInfo("✓ testInvalidRemove OK");
}

void NeedCalculationRegistryTester::testFindByProductAndName()
{
    zInfo("→ testFindByProductAndName");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc1 = makeNC(ids.P1, "ModeA");
    NeedCalculation nc2 = makeNC(ids.P1, "ModeB");

    Q_ASSERT(reg.insert(nc1));
    Q_ASSERT(reg.insert(nc2));

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.contains(NeedCalculationRepository::toCsvLine(nc1)));
    Q_ASSERT(lines.contains(NeedCalculationRepository::toCsvLine(nc2)));

    zInfo("✓ testFindByProductAndName OK");
}

void NeedCalculationRegistryTester::testSubscription()
{
    zInfo("→ testSubscription");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    int counter = 0;

    auto token = reg.subscribeItemsChangedToken([&]() {
        counter++;
    });

    NeedCalculation nc = makeNC(ids.P1, "ModeA");

    Q_ASSERT(reg.insert(nc));
    nc.name = "ModeA2";
    Q_ASSERT(reg.update(nc));
    Q_ASSERT(reg.remove(nc.id));

    Q_ASSERT(counter == 3);

    zInfo("✓ testSubscription OK");
}

bool NeedCalculationRegistryTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    FileNameHelper::instance().setTestMode(true);

    testValidInsert();
    testDuplicateInsert();
    testInvalidDomain();
    testValidUpdate();
    testInvalidUpdateDuplicate();
    testValidRemove();
    testInvalidRemove();
    testFindByProductAndName();
    testSubscription();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
