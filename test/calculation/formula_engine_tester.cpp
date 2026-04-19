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
    testOptSimpleTrue();

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
    Q_ASSERT(v.number() == 1185);

    zInfo("✓ testWidthMinus OK");
}

void FormulaEngineTester::testHeightMinus()
{
    zInfo("→ testHeightMinus");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("h-10");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number() == 1490);

    zInfo("✓ testHeightMinus OK");
}

void FormulaEngineTester::testFixedPieces()
{
    zInfo("→ testFixedPieces");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("qty = 2");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("qty");
    Q_ASSERT(v.number() == 2);

    zInfo("✓ testFixedPieces OK");
}

void FormulaEngineTester::testAreaLike()
{
    zInfo("→ testAreaLike");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("w*h");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number() == 1200 * 1500);

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

    // v1 DSL továbbra is támogatott: requiredLength = w-15, w-10
    auto d1 = TestDataBuilder::makeDetail(
        mode.id, ids.M1,
        "len = w - 15",
        NeedCalculationDetail::DetailKind::Cutting
        );
    auto d2 = TestDataBuilder::makeDetail(
        mode.id, ids.M2,
        "len = w - 10",
        NeedCalculationDetail::DetailKind::Cutting
        );

    NeedCalculationDetailRegistry::instance().insert(d1);
    NeedCalculationDetailRegistry::instance().insert(d2);

    // v2 OrderLine
    OrderLine line;
    line.productId  = ids.P1;
    line.width_mm   = 1200;
    line.height_mm  = 1500;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "Manufacturing", true);

    // 2 RawCut kell legyen
    Q_ASSERT(need.cutItems.size() == 2);

    // A sorrend determinisztikus
    Q_ASSERT(need.cutItems[0].materialBarcode == ids.M1_barcode);
    Q_ASSERT(need.cutItems[1].materialBarcode == ids.M2_barcode);

    Q_ASSERT(need.cutItems[0].requiredLength == 1185); // 1200 - 15
    Q_ASSERT(need.cutItems[1].requiredLength == 1190); // 1200 - 10

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

    // Hibás formula → registry még elfogadja
    NeedCalculationDetail d;
    d.id = QUuid::createUuid();
    d.needCalculationId = mode.id;
    d.materialId = ids.M1;
    d.formula = "w-";   // hibás
    d.kind = NeedCalculationDetail::DetailKind::Cutting;

    bool inserted = NeedCalculationDetailRegistry::instance().insert(d);
    Q_ASSERT(inserted);

    // v2 OrderLine
    OrderLine line;
    line.productId  = ids.P1;
    line.width_mm   = 1200;
    line.height_mm  = 1500;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "Manufacturing", true);

    // Hibás formula → skip → üres eredmény
    Q_ASSERT(need.cutItems.isEmpty());
    Q_ASSERT(need.kitItems.isEmpty());

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
    Q_ASSERT(v.type() == Value::Type::String);
    Q_ASSERT(v.string() == "MOTOR_A");

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
    Q_ASSERT(v.type() == Value::Type::String);
    Q_ASSERT(v.string() == "MOTOR_B");

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

    auto r = FormulaEngine::eval("paint ?? 40");
    r.debugDump();

    Q_ASSERT(r.ok);

    Value v = vars.get("_result");
    Q_ASSERT(v.isNull());

    zInfo("✓ testOptSimple OK");
}

void FormulaEngineTester::testOptSimpleTrue()
{
    setVars(1200, 1500, 1);
    auto& vars = VariableRepository::instance();
    vars.set("paint", Value::boolValue(true));

    auto r = FormulaEngine::eval("paint ?? 40");
    r.debugDump();
    Q_ASSERT(r.ok);

    auto v = vars.get("_result");
    Q_ASSERT(v.number() == 40);
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

    // v2 OrderLine
    OrderLine line;
    line.productId   = ids.P1;
    line.width_mm    = 2000;
    line.height_mm   = 2000;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    // choose: feltétel igaz → M1 barcode
    QString cmd = QString(
                      "qty = 1\n"
                      "material = (w*h > 1000000) ? \"%1\" : \"%2\""
                      ).arg(ids.M1_barcode, ids.M2_barcode);

    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1, cmd, NeedCalculationDetail::DetailKind::Kitting);

    NeedCalculationDetailRegistry::instance().insert(d);

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "Manufacturing", true);

    Q_ASSERT(need.kitItems.size() == 1);

    // choose → M1 barcode
    Q_ASSERT(need.kitItems[0].materialBarcode == ids.M1_barcode);

    // qty = 1 → egy példányra qty = 1
    Q_ASSERT(need.kitItems[0].kitting_qty == 1);

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

    // v2 OrderLine
    OrderLine line;
    line.productId   = ids.P1;
    line.width_mm    = 1000;   // feltétel hamis
    line.height_mm   = 1000;
    line.handlerSide = "";
    line.externalId  = "X";
    line.ownerName   = "";
    line.colorName   = "";

    // choose: feltétel hamis → M2 barcode
    QString cmd = QString(
                      "qty = 1\n"
                      "material = (w*h > 1000000) ? \"%1\" : \"%2\""
                      ).arg(ids.M1_barcode, ids.M2_barcode);

    auto d = TestDataBuilder::makeDetail(
        mode.id, ids.M1, cmd, NeedCalculationDetail::DetailKind::Kitting);

    NeedCalculationDetailRegistry::instance().insert(d);

    // ÚJ API
    ItemNeed need = NeedCalculator::calculate(line, "Manufacturing", true);

    Q_ASSERT(need.kitItems.size() == 1);

    // choose → M2 barcode
    Q_ASSERT(need.kitItems[0].materialBarcode == ids.M2_barcode);

    // qty = 1 → egy példányra qty = 1
    Q_ASSERT(need.kitItems[0].kitting_qty == 1);

    zInfo("✓ testNeedCalculatorChooseFalse OK");
}
