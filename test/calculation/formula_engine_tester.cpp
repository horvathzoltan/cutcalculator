#include "formula_engine_tester.h"

#include "common/logger/logger.h"
#include "test/common/test_data_builder.h"

// Ha van publikus FormulaEngine API, ide húzd be:
#include "expression/formula_engine.h"
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

// A NeedCalculator már a FormulaEngine v2 DSL-t használja.

// ---------------------------------------------------------------------
// 1) Alap DSL minták
// ---------------------------------------------------------------------

void FormulaEngineTester::testWidthMinus()
{
    zInfo("→ testWidthMinus");

    auto ev = FormulaEngine::eval("w-15", 1200, 1500, 1);
    Q_ASSERT(ev.length_mm == 1185);
    Q_ASSERT(ev.pieces == 1);

    zInfo("✓ testWidthMinus OK");
}

void FormulaEngineTester::testHeightMinus()
{
    zInfo("→ testHeightMinus");

    auto ev = FormulaEngine::eval("h-10", 1200, 1500, 1);
    Q_ASSERT(ev.length_mm == 1490);
    Q_ASSERT(ev.pieces == 1);

    zInfo("✓ testHeightMinus OK");
}

void FormulaEngineTester::testFixedPieces()
{
    zInfo("→ testFixedPieces");

    auto ev = FormulaEngine::eval("fixed:2", 1200, 1500, 1);
    Q_ASSERT(ev.pieces == 2);

    zInfo("✓ testFixedPieces OK");
}

void FormulaEngineTester::testAreaLike()
{
    zInfo("→ testAreaLike");

    auto ev = FormulaEngine::eval("w*h", 1200, 1500, 1);
    Q_ASSERT(ev.length_mm == 1200 * 1500);
    Q_ASSERT(ev.pieces == 1);

    zInfo("✓ testAreaLike OK");
}

void FormulaEngineTester::testInvalidEmpty()
{
    zInfo("→ testInvalidEmpty");

    auto ev = FormulaEngine::eval("", 1200, 1500, 1);
    Q_ASSERT(ev.length_mm == 0);
    Q_ASSERT(ev.pieces == 0);

    zInfo("✓ testInvalidEmpty OK");
}

void FormulaEngineTester::testInvalidGarbage()
{
    zInfo("→ testInvalidGarbage");

    auto ev = FormulaEngine::eval("this_is_not_valid", 1200, 1500, 1);
    Q_ASSERT(ev.length_mm == 0);
    Q_ASSERT(ev.pieces == 0);

    zInfo("✓ testInvalidGarbage OK");
}

// ---------------------------------------------------------------------
// 2) Integráció NeedCalculator-ral
// ---------------------------------------------------------------------

void FormulaEngineTester::testNeedCalculatorSimpleRoletta()
{
    zInfo("→ testNeedCalculatorSimpleRoletta");

    // 1) Tiszta indulás
    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    // 2) Tesztadatok létrehozása
    auto ids = TestDataBuilder::prepareStandard(); // P1, M1, M2

    // 3) A ProductRegistry és MaterialRegistry MOSTANTÓL NEM TÖRÖLHETŐ
    //    mert a tesztadatok ezekben vannak


    // Tegyük fel: M1 = tengely, M2 = pálca (a konkrét meaning most mindegy)
    auto& nreg = NeedRuleRegistry::instance();
    Q_ASSERT(nreg.insert(ids.P1, ids.M1));
    Q_ASSERT(nreg.insert(ids.P1, ids.M2));

    // Mode: "Manufacturing"
    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    Q_ASSERT(NeedCalculationRegistry::instance().insert(mode));

    // Details:
    // M1 → w-15
    // M2 → w-10
    auto d1 = TestDataBuilder::makeDetail(mode.id, ids.M1, "w-15");
    auto d2 = TestDataBuilder::makeDetail(mode.id, ids.M2, "w-10");

    auto& reg_nd = NeedCalculationDetailRegistry::instance();
    auto e1 = reg_nd.insert(d1);
    auto e2 = reg_nd.insert(d2);
    Q_ASSERT(e1);
    Q_ASSERT(e2);

    // OrderLine jellegű input – itt csak a lényeg:
    int width  = 1200;
    int height = 1500;
    int qty    = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, width, height, qty }, "Manufacturing");

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

    NeedCalculation mode;
    mode.id = QUuid::createUuid();
    mode.productId = ids.P1;
    mode.name = "Manufacturing";
    Q_ASSERT(NeedCalculationRegistry::instance().insert(mode));

    // Hibás formula: "w-"
    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = mode.id;
    d.materialId = ids.M1;
    d.formula = "w-";
    Q_ASSERT(!NeedCalculationDetailRegistry::instance().insert(d));

    int width  = 1200;
    int height = 1500;
    int qty    = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, width, height, qty }, "Manufacturing");

    Q_ASSERT(cuts.isEmpty());

    zInfo("✓ testNeedCalculatorInvalidFormulaAudit OK");

}


void FormulaEngineTester::testChooseSimple()
{
    zInfo("→ testChooseSimple");

    // nagy terület → igaz ág
    auto ev = FormulaEngine::eval("choose: (w*h > 1000000) ? MOTOR_A : MOTOR_B",
                                  2000, 2000, 1);

    Q_ASSERT(ev.stringValue == "MOTOR_A");
    Q_ASSERT(ev.length_mm == 0);
    Q_ASSERT(ev.pieces == 0);

    zInfo("✓ testChooseSimple OK");
}

void FormulaEngineTester::testChooseFalseBranch()
{
    zInfo("→ testChooseFalseBranch");

    // kis terület → hamis ág
    auto ev = FormulaEngine::eval("choose: (w*h > 5000000) ? MOTOR_A : MOTOR_B",
                                  1000, 1000, 1);

    Q_ASSERT(ev.stringValue == "MOTOR_B");
    Q_ASSERT(ev.length_mm == 0);
    Q_ASSERT(ev.pieces == 0);

    zInfo("✓ testChooseFalseBranch OK");
}

void FormulaEngineTester::testOptSimple()
{
    zInfo("→ testOptSimple");

    // opt: token figyelmen kívül hagyva (flag hiányában)
    auto ev = FormulaEngine::eval("h-10 + opt:paint:+40", 1200, 1500, 1);

    // h-10 = 1490
    // opt:paint:+40 → flag hiányában kihagyva
    Q_ASSERT(ev.length_mm == 1490);
    Q_ASSERT(ev.pieces == 1);

    zInfo("✓ testOptSimple OK");
}

void FormulaEngineTester::testNeedCalculatorChooseTrue()
{
    zInfo("→ testNeedCalculatorChooseTrue");

    // 1) Tiszta indulás
    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    // 2) Tesztadatok
    auto ids = TestDataBuilder::prepareStandard(); // P1, M1, M2

    // 3) NeedRule: P1 → M1, M2
    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    // 4) Mode
    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    NeedCalculationRegistry::instance().insert(mode);

    // 5) choose: formula → MOTOR_A (M1 barcode)
    auto d = TestDataBuilder::makeDetail(mode.id, ids.M1,
                                         QString("choose: (w*h > 1000000) ? %1 : %2")
                                             .arg(ids.M1_barcode, ids.M2_barcode));

    NeedCalculationDetailRegistry::instance().insert(d);

    // 6) OrderLine
    int w = 2000;
    int h = 2000;
    int qty = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, w, h, qty }, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialId == ids.M1);  // MOTOR_A → M1
    Q_ASSERT(cuts[0].pieces == 1);

    zInfo("✓ testNeedCalculatorChooseTrue OK");
}

void FormulaEngineTester::testNeedCalculatorChooseFalse()
{
    zInfo("→ testNeedCalculatorChooseFalse");

    // 1) Tiszta indulás
    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    // 2) Tesztadatok
    auto ids = TestDataBuilder::prepareStandard(); // P1, M1, M2

    // 3) NeedRule: P1 → M1, M2
    NeedRuleRegistry::instance().insert(ids.P1, ids.M1);
    NeedRuleRegistry::instance().insert(ids.P1, ids.M2);

    // 4) Mode
    auto mode = TestDataBuilder::makeCalculation(ids.P1, "Manufacturing");
    NeedCalculationRegistry::instance().insert(mode);

    // 5) choose: formula → MOTOR_B (M2 barcode)
    auto d = TestDataBuilder::makeDetail(mode.id, ids.M1,
                                         QString("choose: (w*h > 5000000) ? %1 : %2")
                                             .arg(ids.M1_barcode, ids.M2_barcode));

    NeedCalculationDetailRegistry::instance().insert(d);

    // 6) OrderLine
    int w = 1000;
    int h = 1000;
    int qty = 1;

    auto cuts = NeedCalculator::makeCutList({ ids.P1, w, h, qty }, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialId == ids.M2);  // MOTOR_B → M2
    Q_ASSERT(cuts[0].pieces == 1);

    zInfo("✓ testNeedCalculatorChooseFalse OK");
}
