#include "need_rule_registry_tester.h"

#include "needs/registry/need_rule_registry.h"
#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"
#include "needs/model/need_rule.h"

#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"

#include "test/common/test_data_builder.h"
#include "test/common/test_file_helper.h"

#include <QFile>
#include <QStringList>

#include <needs/repository/need_rule_repository.h>

class test_NeedRuleRepository {
public:
    static QString toCsvLine(const NeedRule& r) {
        return NeedRuleRepository::toCsvLine(r);
    }
};

void NeedRuleRegistryTester::clearAllRegistries()
{
    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
}

void NeedRuleRegistryTester::prepare()
{
    ids = TestDataBuilder::prepareStandard();
    NeedRuleRegistry::instance().clearForTest();

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::NeedRules, FileAccess::Write);
    QFile::remove(csvPath);
}

void NeedRuleRegistryTester::testValidInsert()
{
    zInfo("→ testValidInsert");
    prepare();

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(reg.size() == 0);
    Q_ASSERT(reg.insert(ids.P1, ids.M1));
    Q_ASSERT(reg.size() == 1);

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::NeedRules, FileAccess::Write);
    QStringList lines = readAllLines(csvPath);

    NeedRule expected(ids.P1, ids.M1);
    QString expectedLine = test_NeedRuleRepository::toCsvLine(expected);

    Q_ASSERT(lines.contains(expectedLine));
}

void NeedRuleRegistryTester::testDuplicateInsert()
{
    zInfo("→ testDuplicateInsert");

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(!reg.insert(ids.P1, ids.M1));
    Q_ASSERT(reg.size() == 1);
}

void NeedRuleRegistryTester::testInvalidLeft()
{
    zInfo("→ testInvalidLeft");

    auto& reg = NeedRuleRegistry::instance();

    QUuid badLeft = QUuid::createUuid();
    Q_ASSERT(!reg.insert(badLeft, ids.M1));
    Q_ASSERT(reg.size() == 1);
}

void NeedRuleRegistryTester::testInvalidRight()
{
    zInfo("→ testInvalidRight");

    auto& reg = NeedRuleRegistry::instance();

    QUuid badRight = QUuid::createUuid();
    Q_ASSERT(!reg.insert(ids.P1, badRight));
    Q_ASSERT(reg.size() == 1);
}

void NeedRuleRegistryTester::testValidRemove()
{
    zInfo("→ testValidRemove");

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(reg.remove(ids.P1, ids.M1));
    Q_ASSERT(reg.size() == 0);

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::NeedRules, FileAccess::Write);
    QStringList lines = readAllLines(csvPath);

    NeedRule removed(ids.P1, ids.M1);
    QString removedLine = test_NeedRuleRepository::toCsvLine(removed);

    Q_ASSERT(!lines.contains(removedLine));
}

void NeedRuleRegistryTester::testInvalidRemove()
{
    zInfo("→ testInvalidRemove");

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(!reg.remove(ids.P1, ids.M2));
    Q_ASSERT(reg.size() == 0);
}

void NeedRuleRegistryTester::testFindByLeft()
{
    zInfo("→ testFindByLeft");

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(reg.insert(ids.P1, ids.M1));
    Q_ASSERT(reg.insert(ids.P1, ids.M2));

    auto v = reg.findByLeft(ids.P1);
    Q_ASSERT(v.size() == 2);

    QString csvPath = FileNameHelper::instance().pathFor(FileKind::NeedRules, FileAccess::Write);
    QStringList lines = readAllLines(csvPath);

    NeedRule r1(ids.P1, ids.M1);
    NeedRule r2(ids.P1, ids.M2);

    Q_ASSERT(lines.contains(test_NeedRuleRepository::toCsvLine(r1)));
    Q_ASSERT(lines.contains(test_NeedRuleRepository::toCsvLine(r2)));
}

void NeedRuleRegistryTester::testRightConvenience()
{
    zInfo("→ testRightConvenience");

    auto& reg = NeedRuleRegistry::instance();

    auto v = reg.findByLeft(ids.P1);
    Q_ASSERT(v.size() == 2);

    for (const auto& r : v) {
        auto opt = r.right();
        Q_ASSERT(opt.has_value());
        Q_ASSERT(opt->id == r.rightId);
    }
}

void NeedRuleRegistryTester::testSubscription()
{
    zInfo("→ testSubscription");

    prepare();

    auto& reg = NeedRuleRegistry::instance();

    int counter = 0;

    auto token = reg.subscribeItemsChangedToken([&]() {
        counter++;
    });

    Q_ASSERT(reg.insert(ids.P1, ids.M1));
    Q_ASSERT(reg.remove(ids.P1, ids.M1));

    Q_ASSERT(counter == 2);
}

bool NeedRuleRegistryTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    FileNameHelper::instance().setTestMode(true);

    testValidInsert();
    testDuplicateInsert();
    testInvalidLeft();
    testInvalidRight();
    testValidRemove();
    testInvalidRemove();
    testFindByLeft();
    testRightConvenience();
    testSubscription();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
