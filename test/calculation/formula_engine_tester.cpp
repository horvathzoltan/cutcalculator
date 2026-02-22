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
    auto r = FormulaEngine::eval("qty_fixed(qty,2)");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
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

    auto d1 = TestDataBuilder::makeDetail(mode.id, ids.M1, "w-15");
    auto d2 = TestDataBuilder::makeDetail(mode.id, ids.M2, "w-10");

    NeedCalculationDetailRegistry::instance().insert(d1);
    NeedCalculationDetailRegistry::instance().insert(d2);

    int width  = 1200;
    int height = 1500;
    int qty    = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, width, height, qty, "" }, "Manufacturing");

    Q_ASSERT(cuts.size() == 2);
    Q_ASSERT(cuts[0].materialId == ids.M1);
    Q_ASSERT(cuts[1].materialId == ids.M2);
    Q_ASSERT(cuts[0].length_mm == 1185);
    Q_ASSERT(cuts[1].length_mm == 1190);

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
    Q_ASSERT(!NeedCalculationDetailRegistry::instance().insert(d));

    int width  = 1200;
    int height = 1500;
    int qty    = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, width, height, qty , ""}, "Manufacturing");

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
    auto r = FormulaEngine::eval("choose: (w*h > 1000000) ? MOTOR_A : MOTOR_B");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.type == Value::Type::String);
    Q_ASSERT(v.text == "MOTOR_A");

    zInfo("✓ testChooseSimple OK");
}

void FormulaEngineTester::testChooseFalseBranch()
{
    zInfo("→ testChooseFalseBranch");

    setVars(1000, 1000, 1);
    auto r = FormulaEngine::eval("choose: (w*h > 5000000) ? MOTOR_A : MOTOR_B");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
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

    auto r = FormulaEngine::eval("h-10 + opt:paint:+40");
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

    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        QString("choose: (w*h > 1000000) ? %1 : %2")
            .arg(ids.M1_barcode, ids.M2_barcode));

    NeedCalculationDetailRegistry::instance().insert(d);

    int w = 2000;
    int h = 2000;
    int qty = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, w, h, qty, "" }, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialId == ids.M1);
    Q_ASSERT(cuts[0].pieces == 1);

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

    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        QString("choose: (w*h > 5000000) ? %1 : %2")
            .arg(ids.M1_barcode, ids.M2_barcode));

    NeedCalculationDetailRegistry::instance().insert(d);

    int w = 1000;
    int h = 1000;
    int qty = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, w, h, qty, "" }, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialId == ids.M2);
    Q_ASSERT(cuts[0].pieces == 1);

    zInfo("✓ testNeedCalculatorChooseFalse OK");
}
