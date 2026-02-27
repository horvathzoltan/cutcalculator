#include "need_calculator_pipeline_tester.h"

#include "calculation/service/need_calculator.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "test/common/test_data_builder.h"

bool NeedCalculatorPipelineTester::run()
{
    zInfo("=== NeedCalculatorPipeline TESTS START ===");

    testLenDsl();
    testQtyDsl();
    testOptDsl();
    testChooseDsl();
    testExplodePieces();
    testAggregation();
    testKitting();

    zInfo("=== NeedCalculatorPipeline TESTS END ===");
    return true;
}

void NeedCalculatorPipelineTester::testLenDsl()
{
    zInfo("→ testLenDsl");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    auto d = TestDataBuilder::makeDetail(mode.id, ids.M1, "len:w-20");
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line{ ids.P1, 1200, 800, 2, "L", "X", "Owner", "Color" };

    auto cuts = NeedCalculator::makeCutList(line, "M");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].requiredLength == 1180);
    Q_ASSERT(cuts[0].quantity == 2);

    zInfo("✓ testLenDsl OK");
}

void NeedCalculatorPipelineTester::testAggregation()
{
    zInfo("→ testAggregation");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    auto d1 = TestDataBuilder::makeDetail(mode.id, ids.M1, "len:w-20");
    auto d2 = TestDataBuilder::makeDetail(mode.id, ids.M1, "len:w-20");
    NeedCalculationDetailRegistry::instance().insert(d1);
    NeedCalculationDetailRegistry::instance().insert(d2);

    OrderLine line{ ids.P1, 1200, 800, 1, "L", "X", "Owner", "Color" };

    auto cuts = NeedCalculator::makeCutList(line, "M");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].quantity == 2);

    zInfo("✓ testAggregation OK");
}


void NeedCalculatorPipelineTester::testQtyDsl()
{
    zInfo("→ testQtyDsl");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    // 🔥 EZ HIÁNYZOTT → kötelező!
    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    auto d = TestDataBuilder::makeDetail(mode.id, ids.M1, "len:w-10 + qty:fixed:3");
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line{ ids.P1, 1200, 800, 2, "L", "X", "Owner", "Color" };

    auto cuts = NeedCalculator::makeCutList(line, "M");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].quantity == 3);
    Q_ASSERT(cuts[0].requiredLength == 1190);

    zInfo("✓ testQtyDsl OK");
}


void NeedCalculatorPipelineTester::testOptDsl()
{
    zInfo("→ testOptDsl");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    // len:w-20 + opt:flag:+40
    auto d = TestDataBuilder::makeDetail(mode.id, ids.M1, "len:w-20 + opt:flag:+40");
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line{ ids.P1, 1200, 800, 1, "L", "X", "Owner", "Color" };

    auto cuts = NeedCalculator::makeCutList(line, "M");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].requiredLength == (1200 - 20 + 40)); // 1220

    zInfo("✓ testOptDsl OK");
}

void NeedCalculatorPipelineTester::testChooseDsl()
{
    zInfo("→ testChooseDsl");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    // choose: w>=1500 ? M1 : M2
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        QString("choose: w>=1500 ? %1 : %2")
            .arg(ids.M1_barcode, ids.M2_barcode));

    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line{ ids.P1, 2000, 800, 1, "L", "X", "Owner", "Color" };

    auto cuts = NeedCalculator::makeCutList(line, "M");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialBarcode == ids.M1_barcode);

    zInfo("✓ testChooseDsl OK");
}

void NeedCalculatorPipelineTester::testExplodePieces()
{
    zInfo("→ testExplodePieces");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    auto d = TestDataBuilder::makeDetail(mode.id, ids.M1, "len:w-10 + qty:fixed:3");
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line{ ids.P1, 1200, 800, 1, "L", "X", "Owner", "Color" };

    auto cuts = NeedCalculator::makeCutList(line, "M");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].quantity == 3);

    // externalRef-ek ellenőrzése
    Q_ASSERT(cuts[0].externalRefs.size() == 3);
    Q_ASSERT(cuts[0].externalRefs[0] == "X.1");
    Q_ASSERT(cuts[0].externalRefs[1] == "X.2");
    Q_ASSERT(cuts[0].externalRefs[2] == "X.3");

    zInfo("✓ testExplodePieces OK");
}

void NeedCalculatorPipelineTester::testKitting()
{
    zInfo("→ testKitting");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    // Kitting sor
    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = mode.id;
    d.materialId = ids.M1;
    d.formula = "qty:perOrder:2";
    d.kind = NeedCalculationDetail::DetailKind::Kitting;

    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line{ ids.P1, 1200, 800, 3, "L", "X", "Owner", "Color" };

    auto kits = NeedCalculator::makeKitList(line, "M");

    Q_ASSERT(kits.size() == 1);
    Q_ASSERT(kits[0].materialId == ids.M1);
    Q_ASSERT(kits[0].quantity == 6); // 3 * 2
    Q_ASSERT(kits[0].fullWidth == 1200);
    Q_ASSERT(kits[0].fullHeight == 800);

    zInfo("✓ testKitting OK");
}

