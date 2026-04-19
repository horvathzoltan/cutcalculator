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
    //testExplodePieces();
    //testAggregation();
    testKitting();
    testCutting();

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

    // ÚJ DSL:
    // requiredLength = w - 20
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        "len = w - 20",
        NeedCalculationDetail::DetailKind::Cutting
        );
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line;
    line.productId = ids.P1;
    line.width_mm = 1200;
    line.height_mm = 800;
    line.handlerSide = "L";
    line.externalId = "X";
    line.ownerName = "Owner";
    line.colorName = "Color";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "M", true);

    // 1 RawCut kell legyen
    Q_ASSERT(need.cutItems.size() == 1);

    // requiredLength = 1200 - 20 = 1180
    Q_ASSERT(need.cutItems[0].requiredLength == 1180);

    // quantity NINCS többé → egy példány BOM-ja
    // a qty majd az OrderItemNeed builderben jelenik meg

    zInfo("✓ testLenDsl OK");
}


// void NeedCalculatorPipelineTester::testAggregation()
// {
//     zInfo("→ testAggregation");

//     ProductRegistry::instance().clearForTest();
//     MaterialRegistry::instance().clearForTest();
//     NeedCalculationRegistry::instance().clearForTest();
//     NeedCalculationDetailRegistry::instance().clearForTest();

//     auto ids = TestDataBuilder::prepareStandard();

//     auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
//     NeedCalculationRegistry::instance().insert(mode);

//     auto d1 = TestDataBuilder::makeDetail(
//         mode.id, ids.M1,
//         "requiredLength = w - 20",
//         NeedCalculationDetail::DetailKind::Cutting
//         );
//     auto d2 = TestDataBuilder::makeDetail(
//         mode.id, ids.M1,
//         "requiredLength = w - 20",
//         NeedCalculationDetail::DetailKind::Cutting
//         );

//     NeedCalculationDetailRegistry::instance().insert(d1);
//     NeedCalculationDetailRegistry::instance().insert(d2);

//     OrderLine line;
//     line.productId = ids.P1;
//     line.width_mm = 1200;
//     line.height_mm = 800;
//     line.handlerSide = "L";
//     line.externalId = "X";
//     line.ownerName = "Owner";
//     line.colorName = "Color";

//     // ÚJ API
//     ItemNeed need = NeedCalculator::calculate(line, "M", true);

//     // 2 RawCut kell legyen (mert 2 detail van)
//     Q_ASSERT(need.cutItems.size() == 2);

//     // Mindkettő requiredLength = 1180
//     Q_ASSERT(need.cutItems[0].requiredLength == 1180);
//     Q_ASSERT(need.cutItems[1].requiredLength == 1180);

//     zInfo("✓ testAggregation OK");
// }


void NeedCalculatorPipelineTester::testQtyDsl()
{
    zInfo("→ testQtyDsl");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    // ÚJ DSL:
    // qty = 3
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        "qty = 3",
        NeedCalculationDetail::DetailKind::Kitting
        );
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line;
    line.productId = ids.P1;
    line.width_mm = 1200;
    line.height_mm = 800;
    line.handlerSide = "L";
    line.externalId = "X";
    line.ownerName = "Owner";
    line.colorName = "Color";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "M", true);

    // 1 RawKit kell legyen
    Q_ASSERT(need.kitItems.size() == 1);

    // qty = 3 → egy példányra 3 db kell
    Q_ASSERT(need.kitItems[0].kitting_qty == 3);

    // materialBarcode is helyes
    Q_ASSERT(need.kitItems[0].materialBarcode == ids.M1_barcode);

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

    // ÚJ DSL:
    // requiredLength = w - 20 + opt: 1 ? +40
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        "len = w - 20 + opt: 1 ? +40",
        NeedCalculationDetail::DetailKind::Cutting
        );
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line;
    line.productId = ids.P1;
    line.width_mm = 1200;
    line.height_mm = 800;
    line.handlerSide = "L";
    line.externalId = "X";
    line.ownerName = "Owner";
    line.colorName = "Color";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "M", true);

    // 1 RawCut kell legyen
    Q_ASSERT(need.cutItems.size() == 1);

    // requiredLength = 1200 - 20 + 40 = 1220
    Q_ASSERT(need.cutItems[0].requiredLength == 1220);

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

    // ÚJ DSL:
    // material = (w >= 1500 ? "M1" : "M2")
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        QString("material = (w >= 1500 ? \"%1\" : \"%2\")")
            .arg(ids.M1_barcode, ids.M2_barcode),
        NeedCalculationDetail::DetailKind::Kitting
        );

    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line;
    line.productId = ids.P1;
    line.width_mm = 2000;   // feltétel igaz
    line.height_mm = 800;
    line.handlerSide = "L";
    line.externalId = "X";
    line.ownerName = "Owner";
    line.colorName = "Color";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "M", true);

    // 1 RawKit kell legyen
    Q_ASSERT(need.kitItems.size() == 1);

    // choose DSL → M1 barcode
    Q_ASSERT(need.kitItems[0].materialBarcode == ids.M1_barcode);

    zInfo("✓ testChooseDsl OK");
}

// void NeedCalculatorPipelineTester::testExplodePieces()
// {
//     zInfo("→ testExplodePieces");

//     ProductRegistry::instance().clearForTest();
//     MaterialRegistry::instance().clearForTest();
//     NeedCalculationRegistry::instance().clearForTest();
//     NeedCalculationDetailRegistry::instance().clearForTest();

//     auto ids = TestDataBuilder::prepareStandard();

//     auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
//     NeedCalculationRegistry::instance().insert(mode);

//     // ÚJ DSL:
//     // requiredLength = w - 10
//     // qty = 3
//     auto d = TestDataBuilder::makeDetail(mode.id, ids.M1,
//                                          "requiredLength = w - 10",
//                                          NeedCalculationDetail::DetailKind::Cutting);
//     NeedCalculationDetailRegistry::instance().insert(d);

//     OrderLine line{ ids.P1, 1200, 800, 1, "L", "X", "Owner", "Color" };

//     auto cuts = NeedCalculator::makeCutList(line, "M");

//     Q_ASSERT(cuts.size() == 1);
//     Q_ASSERT(cuts[0].quantity == 3);

//     Q_ASSERT(cuts[0].externalRefs.size() == 3);
//     Q_ASSERT(cuts[0].externalRefs[0] == "X.1");
//     Q_ASSERT(cuts[0].externalRefs[1] == "X.2");
//     Q_ASSERT(cuts[0].externalRefs[2] == "X.3");

//     zInfo("✓ testExplodePieces OK");
// }

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

    // ÚJ DSL:
    // qty = qty * 2
    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = mode.id;
    d.materialId = ids.M1;
    d.formula = "qty = qty * 2";
    d.kind = NeedCalculationDetail::DetailKind::Kitting;

    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line;
    line.productId = ids.P1;
    line.width_mm = 1200;
    line.height_mm = 800;
    line.handlerSide = "L";
    line.externalId = "X";
    line.ownerName = "Owner";
    line.colorName = "Color";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "M", true);

    // 1 RawKit kell legyen
    Q_ASSERT(need.kitItems.size() == 1);

    // qty = 2 → egy példányra 2 db kell
    Q_ASSERT(need.kitItems[0].kitting_qty == 2);

    // materialBarcode is helyes
    Q_ASSERT(need.kitItems[0].materialBarcode == ids.M1_barcode);

    zInfo("✓ testKitting OK");
}

void NeedCalculatorPipelineTester::testCutting()
{
    zInfo("→ testCutting");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "M");
    NeedCalculationRegistry::instance().insert(mode);

    // Cutting DSL:
    // requiredLength = w - 30
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        "len = w - 30",
        NeedCalculationDetail::DetailKind::Cutting
        );
    NeedCalculationDetailRegistry::instance().insert(d);

    OrderLine line;
    line.productId = ids.P1;
    line.width_mm = 1500;
    line.height_mm = 800;
    line.handlerSide = "L";
    line.externalId = "X";
    line.ownerName = "Owner";
    line.colorName = "Color";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "M", true);

    // 1 RawCut kell legyen
    Q_ASSERT(need.cutItems.size() == 1);

    // requiredLength = 1500 - 30 = 1470
    Q_ASSERT(need.cutItems[0].requiredLength == 1470);

    // materialBarcode is helyes
    Q_ASSERT(need.cutItems[0].materialBarcode == ids.M1_barcode);

    zInfo("✓ testCutting OK");
}


