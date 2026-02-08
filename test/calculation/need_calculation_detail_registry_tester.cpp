#include "need_calculation_detail_registry_tester.h"

#include "calculation/registry/need_calculation_detail_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"

#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include "test/common/test_data_builder.h"
#include "test/common/test_file_helper.h"

#include <QStringList>
#include <QFile>

NeedCalculationDetail NeedCalculationDetailRegistryTester::makeD(
    const QUuid& calcId,
    const QUuid& materialId,
    const QString& formula)
{
    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = calcId;
    d.materialId = materialId;
    d.formula = formula;
    return d;
}

void NeedCalculationDetailRegistryTester::prepare()
{
    ids = TestDataBuilder::prepareStandard();

    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QFile::remove(csv);

    NeedCalculation nc;
    nc.id = QUuid::createUuid();
    nc.productId = ids.P1;
    nc.name = "ModeA";
    NeedCalculationRegistry::instance().insert(nc);
}

void NeedCalculationDetailRegistryTester::testValidInsert()
{
    zInfo("→ testValidInsert");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail d = makeD(calc->id, ids.M1, "w-10");
    Q_ASSERT(reg.insert(d));
    Q_ASSERT(reg.size() == 1);

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList lines = readAllLines(csv);
    Q_ASSERT(lines.size() == 2);

    zInfo("✓ testValidInsert OK");
}

void NeedCalculationDetailRegistryTester::testDuplicateInsert()
{
    zInfo("→ testDuplicateInsert");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail d1 = makeD(calc->id, ids.M1, "w-10");
    NeedCalculationDetail d2 = makeD(calc->id, ids.M1, "w-10");

    Q_ASSERT(reg.insert(d1));

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList before = readAllLines(csv);

    Q_ASSERT(!reg.insert(d2));

    QStringList after = readAllLines(csv);
    Q_ASSERT(before == after);

    zInfo("✓ testDuplicateInsert OK");
}

void NeedCalculationDetailRegistryTester::testInvalidDomain()
{
    zInfo("→ testInvalidDomain");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail bad = makeD(calc->id, ids.M1, "");
    Q_ASSERT(!reg.insert(bad));

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList lines = readAllLines(csv);
    Q_ASSERT(lines.isEmpty());

    zInfo("✓ testInvalidDomain OK");
}

void NeedCalculationDetailRegistryTester::testValidUpdate()
{
    zInfo("→ testValidUpdate");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail d = makeD(calc->id, ids.M1, "w-10");
    Q_ASSERT(reg.insert(d));

    d.formula = "h-20";
    Q_ASSERT(reg.update(d));

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList lines = readAllLines(csv);

    Q_ASSERT(lines.contains("P1;ModeA;M1;h-20"));

    zInfo("✓ testValidUpdate OK");
}

void NeedCalculationDetailRegistryTester::testInvalidUpdateDuplicate()
{
    zInfo("→ testInvalidUpdateDuplicate");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail d1 = makeD(calc->id, ids.M1, "w-10");
    NeedCalculationDetail d2 = makeD(calc->id, ids.M2, "h-5");

    Q_ASSERT(reg.insert(d1));
    Q_ASSERT(reg.insert(d2));

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList before = readAllLines(csv);

    d2.materialId = ids.M1;
    d2.formula = "w-10";

    Q_ASSERT(!reg.update(d2));

    QStringList after = readAllLines(csv);
    Q_ASSERT(before == after);

    zInfo("✓ testInvalidUpdateDuplicate OK");
}

void NeedCalculationDetailRegistryTester::testValidRemove()
{
    zInfo("→ testValidRemove");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail d = makeD(calc->id, ids.M1, "w-10");
    Q_ASSERT(reg.insert(d));

    Q_ASSERT(reg.remove(d.id));
    Q_ASSERT(reg.size() == 0);

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList lines = readAllLines(csv);
    Q_ASSERT(lines.size() == 1);

    zInfo("✓ testValidRemove OK");
}

void NeedCalculationDetailRegistryTester::testInvalidRemove()
{
    zInfo("→ testInvalidRemove");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();

    QString csv = FileNameHelper::instance().pathFor((FileKind::NeedCalculationDetails), FileAccess::Write);
    QStringList before = readAllLines(csv);

    Q_ASSERT(!reg.remove(QUuid::createUuid()));

    QStringList after = readAllLines(csv);
    Q_ASSERT(before == after);

    zInfo("✓ testInvalidRemove OK");
}

void NeedCalculationDetailRegistryTester::testFindByCalculation()
{
    zInfo("→ testFindByCalculation");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    NeedCalculationDetail d1 = makeD(calc->id, ids.M1, "w-10");
    NeedCalculationDetail d2 = makeD(calc->id, ids.M2, "h-5");

    Q_ASSERT(reg.insert(d1));
    Q_ASSERT(reg.insert(d2));

    auto list = reg.findByCalculation(calc->id);
    Q_ASSERT(list.size() == 2);

    zInfo("✓ testFindByCalculation OK");
}

void NeedCalculationDetailRegistryTester::testSubscription()
{
    zInfo("→ testSubscription");
    prepare();

    auto& reg = NeedCalculationDetailRegistry::instance();
    auto& calcReg = NeedCalculationRegistry::instance();

    const auto* calc = calcReg.findIf([](const NeedCalculation& nc){
        return nc.name == "ModeA";
    });
    Q_ASSERT(calc);

    int counter = 0;
    auto token = reg.subscribeItemsChangedToken([&](){
        counter++;
    });

    NeedCalculationDetail d = makeD(calc->id, ids.M1, "w-10");

    Q_ASSERT(reg.insert(d));
    d.formula = "h-20";
    Q_ASSERT(reg.update(d));
    Q_ASSERT(reg.remove(d.id));

    Q_ASSERT(counter == 3);

    zInfo("✓ testSubscription OK");
}

bool NeedCalculationDetailRegistryTester::run()
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
    testFindByCalculation();
    testSubscription();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
