#include "matrix_validator_tester.h"

#include "calculation/service/matrix_validator.h"

#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"

#include "test/common/test_data_builder.h"
#include "common/logger/logger.h"

bool MatrixValidatorTester::run()
{
    zInfo("=== MatrixValidatorTester START ===");

    testEmptyMatrix();
    testSingleRuleMissingDetail();
    testSingleRuleComplete();
    testMultipleRulesComplete();
    testMultipleRulesIncomplete();

    zInfo("=== MatrixValidatorTester END ===");
    return true;
}

// ------------------------------------------------------------
// 1) Üres mátrix → teljesnek számít
// ------------------------------------------------------------
void MatrixValidatorTester::testEmptyMatrix()
{
    zInfo("→ testEmptyMatrix");

    clearAll();

    auto ids = TestDataBuilder::prepareStandard();

    bool ok = MatrixValidator::isProductMatrixComplete(ids.P1);
    Q_ASSERT(ok == false);

    zInfo("✓ testEmptyMatrix OK");
}

// ------------------------------------------------------------
// 2) Egy szabály, nincs detail → hiányos
// ------------------------------------------------------------
void MatrixValidatorTester::testSingleRuleMissingDetail()
{
    zInfo("→ testSingleRuleMissingDetail");

    clearAll();
    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);

    NeedCalculation nc;
    nc.id = QUuid::createUuid();
    nc.productId = ids.P1;
    nc.name = "ModeA";
    NeedCalculationRegistry::instance().insert(nc);

    bool ok = MatrixValidator::isProductMatrixComplete(ids.P1);
    Q_ASSERT(ok == false);

    zInfo("✓ testSingleRuleMissingDetail OK");
}

// ------------------------------------------------------------
// 3) Egy szabály, egy detail → teljes
// ------------------------------------------------------------
void MatrixValidatorTester::testSingleRuleComplete()
{
    zInfo("→ testSingleRuleComplete");

    clearAll();
    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);

    NeedCalculation nc;
    nc.id = QUuid::createUuid();
    nc.productId = ids.P1;
    nc.name = "ModeA";
    NeedCalculationRegistry::instance().insert(nc);

    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = nc.id;
    d.materialId = ids.M1;
    d.formula = "len:w-10";
    NeedCalculationDetailRegistry::instance().insert(d);

    bool ok = MatrixValidator::isProductMatrixComplete(ids.P1);
    Q_ASSERT(ok == true);

    zInfo("✓ testSingleRuleComplete OK");
}

// ------------------------------------------------------------
// 4) Több szabály + több mode → minden detail megvan → teljes
// ------------------------------------------------------------
void MatrixValidatorTester::testMultipleRulesComplete()
{
    zInfo("→ testMultipleRulesComplete");

    clearAll();
    auto ids = TestDataBuilder::prepareStandard();

    // P1 → M1, M2
    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    // ModeA + ModeB
    NeedCalculation modeA;
    modeA.id = QUuid::createUuid();
    modeA.productId = ids.P1;
    modeA.name = "ModeA";
    NeedCalculationRegistry::instance().insert(modeA);

    NeedCalculation modeB;
    modeB.id = QUuid::createUuid();
    modeB.productId = ids.P1;
    modeB.name = "ModeB";
    NeedCalculationRegistry::instance().insert(modeB);

    // Minden kombinációhoz detail
    auto addDetail = [&](QUuid modeId, QUuid matId) {
        NeedCalculationDetail d;
        d.id = QUuid::createUuid();
        d.needCalculationId = modeId;
        d.materialId = matId;
        d.formula = "len:w-10";
        NeedCalculationDetailRegistry::instance().insert(d);
    };

    addDetail(modeA.id, ids.M1);
    addDetail(modeA.id, ids.M2);
    addDetail(modeB.id, ids.M1);
    addDetail(modeB.id, ids.M2);

    bool ok = MatrixValidator::isProductMatrixComplete(ids.P1);
    Q_ASSERT(ok == true);

    zInfo("✓ testMultipleRulesComplete OK");
}

// ------------------------------------------------------------
// 5) Több szabály + több mode → egy detail hiányzik → hiányos
// ------------------------------------------------------------
void MatrixValidatorTester::testMultipleRulesIncomplete()
{
    zInfo("→ testMultipleRulesIncomplete");

    clearAll();
    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    NeedCalculation modeA;
    modeA.id = QUuid::createUuid();
    modeA.productId = ids.P1;
    modeA.name = "ModeA";
    NeedCalculationRegistry::instance().insert(modeA);

    NeedCalculation modeB;
    modeB.id = QUuid::createUuid();
    modeB.productId = ids.P1;
    modeB.name = "ModeB";
    NeedCalculationRegistry::instance().insert(modeB);

    // Minden detail megvan, kivéve egyet
    auto addDetail = [&](QUuid modeId, QUuid matId) {
        NeedCalculationDetail d;
        d.id = QUuid::createUuid();
        d.needCalculationId = modeId;
        d.materialId = matId;
        d.formula = "len:w-10";
        NeedCalculationDetailRegistry::instance().insert(d);
    };

    addDetail(modeA.id, ids.M1);
    addDetail(modeA.id, ids.M2);
    addDetail(modeB.id, ids.M1);
    // modeB + M2 → HIÁNYZIK

    bool ok = MatrixValidator::isProductMatrixComplete(ids.P1);
    Q_ASSERT(ok == false);

    zInfo("✓ testMultipleRulesIncomplete OK");
}

// ------------------------------------------------------------
// Helper
// ------------------------------------------------------------
void MatrixValidatorTester::clearAll()
{
    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();
}
