#include "formula_engine_tester.h"

#include "common/logger/logger.h"
#include "test/common/test_data_builder.h"

#include "expression/formula_engine.h"
#include "expression/variable.h"
#include "calculation/service/need_calculator.h"

#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"
#include "common/utils/filename_helper.h"

// ---------------------------------------------------------------------
// RUN
// ---------------------------------------------------------------------

bool FormulaEngineTester::run()
{
    zInfo("=== FormulaEngine TESTS START ===");

    testWidthMinus();
    testHeightMinus();
    testFixedPieces();
    testAreaLike();
    testInvalidEmpty();
    testInvalidGarbage();

    testNeedCalculatorSimpleRoletta();
    testNeedCalculatorInvalidFormulaAudit();

    testChooseSimple();
    testChooseFalseBranch();
    testOptSimple();

    testNeedCalculatorChooseTrue();
    testNeedCalculatorChooseFalse();

    zInfo("=== FormulaEngine TESTS END ===");
    return true;
}

void FormulaEngineTester::setVars(int w, int h, int qty)
{
    auto& vars = VariableRepository::instance();
    vars.clear();
    vars.set("w", Value::numberValue(w));
    vars.set("h", Value::numberValue(h));
    vars.set("qty", Value::numberValue(qty));
}

// ---------------------------------------------------------------------
// 1) Alap DSL minták
// ---------------------------------------------------------------------

void FormulaEngineTester::testWidthMinus()
{
    zInfo("→ testWidthMinus");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("w-15");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number == 1185);

    zInfo("✓ testWidthMinus OK");
}

void FormulaEngineTester::testHeightMinus()
{
    zInfo("→ testHeightMinus");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("h-10");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number == 1490);

    zInfo("✓ testHeightMinus OK");
}

void FormulaEngineTester::testFixedPieces()
{
    zInfo("→ testFixedPieces");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("qty = 2");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("qty");
    Q_ASSERT(v.number == 2);

    zInfo("✓ testFixedPieces OK");
}

void FormulaEngineTester::testAreaLike()
{
    zInfo("→ testAreaLike");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("w*h");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number == 1200 * 1500);

    zInfo("✓ testAreaLike OK");
}

void FormulaEngineTester::testInvalidEmpty()
{
    zInfo("→ testInvalidEmpty");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("");

    Q_ASSERT(!r.ok);

    zInfo("✓ testInvalidEmpty OK");
}

void FormulaEngineTester::testInvalidGarbage()
{
    zInfo("→ testInvalidGarbage");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("this_is_not_valid");

    Q_ASSERT(!r.ok);

    zInfo("✓ testInvalidGarbage OK");
}

// ---------------------------------------------------------------------
// 2) Integráció NeedCalculator-ral
// ---------------------------------------------------------------------

void FormulaEngineTester::testNeedCalculatorSimpleRoletta()
{
    zInfo("→ testNeedCalculatorSimpleRoletta");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    NeedCalculationRegistry::instance().insert(mode);

    // v1 DSL továbbra is támogatott: w-15, w-10
    auto d1 = TestDataBuilder::makeDetail(mode.id, ids.M1, "requiredLength = w-15", NeedCalculationDetail::DetailKind::Cutting);
    auto d2 = TestDataBuilder::makeDetail(mode.id, ids.M2, "requiredLength = w-10", NeedCalculationDetail::DetailKind::Cutting);

    NeedCalculationDetailRegistry::instance().insert(d1);
    NeedCalculationDetailRegistry::instance().insert(d2);

    // v2 OrderLine
    OrderLine line;
    line.productId  = ids.P1;
    line.width_mm   = 1200;
    line.height_mm  = 1500;
    line.qty        = 1;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    auto cuts = NeedCalculator::makeCutList(line, "Manufacturing");

    Q_ASSERT(cuts.size() == 2);

    // A sorrend determinisztikus (QMap → QVector)
    Q_ASSERT(cuts[0].materialBarcode == ids.M1_barcode);
    Q_ASSERT(cuts[1].materialBarcode == ids.M2_barcode);

    Q_ASSERT(cuts[0].requiredLength == 1185); // 1200 - 15
    Q_ASSERT(cuts[1].requiredLength == 1190); // 1200 - 10

    zInfo("✓ testNeedCalculatorSimpleRoletta OK");
}


void FormulaEngineTester::testNeedCalculatorInvalidFormulaAudit()
{
    zInfo("→ testNeedCalculatorInvalidFormulaAudit");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    NeedCalculationRegistry::instance().insert(mode);

    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = mode.id;
    d.materialId = ids.M1;
    d.formula = "w-";

    bool inserted = NeedCalculationDetailRegistry::instance().insert(d);
    Q_ASSERT(inserted);

    int width  = 1200;
    int height = 1500;
    int qty    = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, width, height, qty , "", "", "", ""}, "Manufacturing");

    Q_ASSERT(cuts.isEmpty());

    zInfo("✓ testNeedCalculatorInvalidFormulaAudit OK");
}

// ---------------------------------------------------------------------
// 3) choose: DSL
// ---------------------------------------------------------------------

void FormulaEngineTester::testChooseSimple()
{
    zInfo("→ testChooseSimple");

    setVars(2000, 2000, 1);
    auto r = FormulaEngine::eval("a1 = (w*h > 1000000) ? \"MOTOR_A\" : \"MOTOR_B\"");
    r.debugDump();

    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("a1");
    Q_ASSERT(v.type == Value::Type::String);
    Q_ASSERT(v.text == "MOTOR_A");

    zInfo("✓ testChooseSimple OK");
}

void FormulaEngineTester::testChooseFalseBranch()
{
    zInfo("→ testChooseFalseBranch");

    setVars(1000, 1000, 1);
    auto r = FormulaEngine::eval("a1 = (w*h > 5000000) ? \"MOTOR_A\" : \"MOTOR_B\"");
    r.debugDump();

    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("a1");
    Q_ASSERT(v.type == Value::Type::String);
    Q_ASSERT(v.text == "MOTOR_B");

    zInfo("✓ testChooseFalseBranch OK");
}

// ---------------------------------------------------------------------
// 4) opt: DSL
// ---------------------------------------------------------------------

void FormulaEngineTester::testOptSimple()
{
    zInfo("→ testOptSimple");

    setVars(1200, 1500, 1);
    auto& vars = VariableRepository::instance();
    vars.set("paint", Value::boolValue(false));

    auto r = FormulaEngine::eval("h-10 + paint ? 40");
    r.debugDump();

    Q_ASSERT(r.ok);

    auto v = vars.get("_result");
    Q_ASSERT(v.number == 1490);

    zInfo("✓ testOptSimple OK");
}

// ---------------------------------------------------------------------
// 5) NeedCalculator + choose
// ---------------------------------------------------------------------

void FormulaEngineTester::testNeedCalculatorChooseTrue()
{
    zInfo("→ testNeedCalculatorChooseTrue");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    NeedCalculationRegistry::instance().insert(mode);

    // choose: feltétel igaz → M1 barcode
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        QString("(w*h > 1000000) ? %1 : %2")
            .arg(ids.M1_barcode, ids.M2_barcode), NeedCalculationDetail::DetailKind::Kitting);

    NeedCalculationDetailRegistry::instance().insert(d);

    // v2 OrderLine
    OrderLine line;
    line.productId   = ids.P1;
    line.width_mm    = 2000;
    line.height_mm   = 2000;
    line.qty         = 1;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    auto cuts = NeedCalculator::makeKitList(line, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);

    // choose → M1 barcode
    Q_ASSERT(cuts[0].materialBarcode == ids.M1_barcode);

    // qty = 1 → quantity = 1
    Q_ASSERT(cuts[0].quantity == 1);

    zInfo("✓ testNeedCalculatorChooseTrue OK");
}


void FormulaEngineTester::testNeedCalculatorChooseFalse()
{
    zInfo("→ testNeedCalculatorChooseFalse");

    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    auto ids = TestDataBuilder::prepareStandard();

    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    NeedCalculationRegistry::instance().insert(mode);

    // choose: feltétel hamis → M2 barcode
    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        QString("(w*h > 5000000) ? %1 : %2")
            .arg(ids.M1_barcode, ids.M2_barcode), NeedCalculationDetail::DetailKind::Kitting);

    NeedCalculationDetailRegistry::instance().insert(d);

    // v2 OrderLine
    OrderLine line;
    line.productId   = ids.P1;
    line.width_mm    = 1000;
    line.height_mm   = 1000;
    line.qty         = 1;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    QVector<KitAggregatedItem> cuts = NeedCalculator::makeKitList(line, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);

    // choose → M2 barcode
    Q_ASSERT(cuts[0].materialBarcode == ids.M2_barcode);

    // qty = 1 → quantity = 1
    Q_ASSERT(cuts[0].quantity == 1);

    zInfo("✓ testNeedCalculatorChooseFalse OK");
}

