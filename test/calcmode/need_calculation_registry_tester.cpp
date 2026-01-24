#include "need_calculation_registry_tester.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/model/need_calculation.h"
#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"
#include "test/common/test_data_builder.h"

#include <QFile>
#include <QStringList>

#include <calcmodes/repository/need_calculation_repository.h>


class test_NeedCalculationRepository {
public:
    static QString toCsvLine(const NeedCalculation& nc) {
        return NeedCalculationRepository::toCsvLine(nc);
    }
};


// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

static TestDataIds ids;

static NeedCalculation makeNC(const QUuid& productId,
                              const QString& name)
{
    NeedCalculation nc;
    nc.id = QUuid::createUuid();
    nc.productId = productId;
    nc.name = name;
    return nc;
}

static void prepare()
{
    ids = TestDataBuilder::prepareStandard();
    NeedCalculationRegistry::instance().clear();

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QFile::remove(csvPath);
}


// ------------------------------------------------------------
// TEST 1: Valid insert
// ------------------------------------------------------------

static void testValidInsert()
{
    zInfo("→ testValidInsert");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc = makeNC(ids.P1, "ModeA");

    Q_ASSERT(reg.insert(nc));
    Q_ASSERT(reg.size() == 1);

    // --- CSV ellenőrzés ---
    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    QString expected = test_NeedCalculationRepository::toCsvLine(nc);
    Q_ASSERT(lines.contains(expected));

    zInfo("✓ testValidInsert OK");
}

// ------------------------------------------------------------
// TEST 2: Duplicate insert tiltása
// ------------------------------------------------------------

static void testDuplicateInsert()
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

// ------------------------------------------------------------
// TEST 3: Invalid domain (üres név)
// ------------------------------------------------------------

static void testInvalidDomain()
{
    zInfo("→ testInvalidDomain");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation bad = makeNC(ids.P1, "");
    Q_ASSERT(!reg.insert(bad));

    QString csvPath = FileNameHelper::instance().getNeedCalculationCsvFile();
    QStringList lines = readAllLines(csvPath);

    Q_ASSERT(lines.size() == 1); // csak header

    zInfo("✓ testInvalidDomain OK");
}


// ------------------------------------------------------------
// TEST 4: Valid update
// ------------------------------------------------------------

static void testValidUpdate()
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

    QString expected = test_NeedCalculationRepository::toCsvLine(nc);
    Q_ASSERT(lines.contains(expected));

    zInfo("✓ testValidUpdate OK");
}


// ------------------------------------------------------------
// TEST 5: Invalid update (duplicate név)
// ------------------------------------------------------------

static void testInvalidUpdateDuplicate()
{
    zInfo("→ testInvalidUpdateDuplicate");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    NeedCalculation nc1 = makeNC(ids.P1, "ModeA");
    NeedCalculation nc2 = makeNC(ids.P1, "ModeB");

    Q_ASSERT(reg.insert(nc1));
    Q_ASSERT(reg.insert(nc2));

    // nc2-t átnevezzük ModeA-ra → duplikáció
    nc2.name = "ModeA";
    Q_ASSERT(!reg.update(nc2));

    zInfo("✓ testInvalidUpdateDuplicate OK");
}

// ------------------------------------------------------------
// TEST 6: Valid remove
// ------------------------------------------------------------

static void testInvalidUpdateDuplicate()
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


// ------------------------------------------------------------
// TEST 7: Invalid remove
// ------------------------------------------------------------

static void testValidRemove()
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

    QString removed = test_NeedCalculationRepository::toCsvLine(nc);
    Q_ASSERT(!lines.contains(removed));

    zInfo("✓ testValidRemove OK");
}
static void testInvalidRemove()
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



// ------------------------------------------------------------
// TEST 8: findByProductAndName
// ------------------------------------------------------------

static void testFindByProductAndName()
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

    Q_ASSERT(lines.contains(test_NeedCalculationRepository::toCsvLine(nc1)));
    Q_ASSERT(lines.contains(test_NeedCalculationRepository::toCsvLine(nc2)));

    zInfo("✓ testFindByProductAndName OK");
}



// ------------------------------------------------------------
// TEST 9: Subscription működése
// ------------------------------------------------------------

static void testSubscription()
{
    zInfo("→ testSubscription");
    prepare();

    auto& reg = NeedCalculationRegistry::instance();

    int counter = 0;

    auto token = reg.subscribeItemsChangedToken([&]() {
        counter++;
    });

    NeedCalculation nc = makeNC(ids.P1, "ModeA");

    Q_ASSERT(reg.insert(nc));   // counter +1
    nc.name = "ModeA2";
    Q_ASSERT(reg.update(nc));   // counter +1
    Q_ASSERT(reg.remove(nc.id)); // counter +1

    Q_ASSERT(counter == 3);

    zInfo("✓ testSubscription OK");
}

// ------------------------------------------------------------
// Entry point
// ------------------------------------------------------------

bool runNeedCalculationRegistryTests()
{
    zInfo("=== NeedCalculationRegistry TESTS START ===");

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

    zInfo("=== NeedCalculationRegistry TESTS END ===");
    return true;
}
