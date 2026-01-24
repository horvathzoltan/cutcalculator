#include "need_rule_registry_tester.h"

#include "needs/registry/need_rule_registry.h"
#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"
#include "needs/model/need_rule.h"

#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"

#include <QFile>
#include <QStringList>

#include "test/common/test_data_builder.h"

#include <needs/repository/need_rule_repository.h>


class test_NeedRuleRepository {
public:
    static QString toCsvLine(const NeedRule& r) {
        return NeedRuleRepository::toCsvLine(r);
    }
};

// --- Helpers -------------------------------------------------------------

static void clearAllRegistries()
{
    ProductRegistry::instance().clear();
    MaterialRegistry::instance().clear();
    NeedRuleRegistry::instance().clear();
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

static TestDataIds ids;


static void prepare()
{
    ids = TestDataBuilder::prepareStandard();
    NeedRuleRegistry::instance().clear();

    QString csvPath = FileNameHelper::instance().getNeedRuleCsvFile();
    QFile::remove(csvPath);
}


// --- Test skeletons ------------------------------------------------------

static void testValidInsert()
{
    zInfo("→ testValidInsert");
    prepare();

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(reg.size() == 0);

    // valid kapcsolat
    Q_ASSERT(reg.insertRule(ids.P1, ids.M1));

    Q_ASSERT(reg.size() == 1);

    // --- CSV ellenőrzés ---
    QString csvPath = FileNameHelper::instance().getNeedRuleCsvFile();
    QStringList lines = readAllLines(csvPath);

    NeedRule expected(ids.P1, ids.M1);
    QString expectedLine = test_NeedRuleRepository::toCsvLine(expected);

    Q_ASSERT(lines.contains(expectedLine));

    zInfo("✓ testValidInsert OK");
}



static void testDuplicateInsert()
{
    zInfo("→ testDuplicateInsert");

    auto& reg = NeedRuleRegistry::instance();

    // már létezik: (P1, M1)
    Q_ASSERT(!reg.insertRule(ids.P1, ids.M1));   // duplikáció → false

    Q_ASSERT(reg.size() == 1);

    // QString csvPath = FileNameHelper::instance().getNeedRuleCsvFile();
    // QStringList lines = readAllLines(csvPath);
    // Q_ASSERT(lines.size() == 1); // csak a header + 1 sor

    zInfo("✓ testDuplicateInsert OK");
}


static void testInvalidLeft()
{
    zInfo("→ testInvalidLeft");

    auto& reg = NeedRuleRegistry::instance();

    QUuid badLeft = QUuid::createUuid();

    Q_ASSERT(!reg.insertRule(badLeft, ids.M1));   // bal oldal nem létezik

    Q_ASSERT(reg.size() == 1); // továbbra is csak (P1, M1)

    zInfo("✓ testInvalidLeft OK");
}


static void testInvalidRight()
{
    zInfo("→ testInvalidRight");

    auto& reg = NeedRuleRegistry::instance();

    QUuid badRight = QUuid::createUuid();

    Q_ASSERT(!reg.insertRule(ids.P1, badRight));  // jobb oldal nem létezik

    Q_ASSERT(reg.size() == 1);

    zInfo("✓ testInvalidRight OK");
}

static void testValidRemove()
{
    zInfo("→ testValidRemove");

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(reg.removeRule(ids.P1, ids.M1));   // létező kapcsolat

    Q_ASSERT(reg.size() == 0);

    // --- CSV ellenőrzés ---
    QString csvPath = FileNameHelper::instance().getNeedRuleCsvFile();
    QStringList lines = readAllLines(csvPath);

    NeedRule removed(ids.P1, ids.M1);
    QString removedLine = test_NeedRuleRepository::toCsvLine(removed);

    Q_ASSERT(!lines.contains(removedLine));

    zInfo("✓ testValidRemove OK");
}



static void testInvalidRemove()
{
    zInfo("→ testInvalidRemove");

    auto& reg = NeedRuleRegistry::instance();

    // nincs ilyen kapcsolat
    Q_ASSERT(!reg.removeRule(ids.P1, ids.M2));

    Q_ASSERT(reg.size() == 0);

    zInfo("✓ testInvalidRemove OK");
}

static void testFindByLeft()
{
    zInfo("→ testFindByLeft");

    auto& reg = NeedRuleRegistry::instance();

    Q_ASSERT(reg.insertRule(ids.P1, ids.M1));
    Q_ASSERT(reg.insertRule(ids.P1, ids.M2));

    auto v = reg.findByLeft(ids.P1);
    Q_ASSERT(v.size() == 2);

    // --- CSV ellenőrzés ---
    QString csvPath = FileNameHelper::instance().getNeedRuleCsvFile();
    QStringList lines = readAllLines(csvPath);

    NeedRule r1(ids.P1, ids.M1);
    NeedRule r2(ids.P1, ids.M2);

    Q_ASSERT(lines.contains(test_NeedRuleRepository::toCsvLine(r1)));
    Q_ASSERT(lines.contains(test_NeedRuleRepository::toCsvLine(r2)));

    zInfo("✓ testFindByLeft OK");
}



static void testRightConvenience()
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

    zInfo("✓ testRightConvenience OK");
}

static void testSubscription()
{
    zInfo("→ testSubscription");

    prepare();

    auto& reg = NeedRuleRegistry::instance();

    int counter = 0;

    auto token = reg.subscribeItemsChangedToken([&]() {
        counter++;
    });

    Q_ASSERT(reg.insertRule(ids.P1, ids.M1));   // counter +1
    Q_ASSERT(reg.removeRule(ids.P1, ids.M1));   // counter +1

    Q_ASSERT(counter == 2);

    zInfo("✓ testSubscription OK");
}


// --- Entry point ---------------------------------------------------------

bool runNeedRuleRegistryTests()
{
    zInfo("=== NeedRuleRegistry TESTS START ===");

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

    zInfo("=== NeedRuleRegistry TESTS END ===");
    return true;
}

