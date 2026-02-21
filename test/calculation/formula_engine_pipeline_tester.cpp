
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
#include "formula_engine_pipeline_tester.h"

// ---------------------------------------------------------------------
// RUN
// ---------------------------------------------------------------------

bool FormulaEnginePipelineTester::run()
{
    zInfo("=== FormulaEngine TESTS START ===");

    // 1) Alap DSL
    testLiteralInt();
    testLiteralDouble();
    testSimpleExpression();
    testAssignment();
    testMultiLine();

    // 2) DSL hibák
    testInvalidEmpty();
    testInvalidGarbage();
    testUndefinedVariable();
    testUndefinedFunction();
    testDivisionByZero();

    testNestedFunctionCall();

    // 3) choose / opt (modern prefix DSL)
    testChooseSimple();
    testChooseNested();
    testChooseFalseBranch();
    testOptSimple();
    testOptExpression();

    // 4) NeedCalculator integráció
    testNeedCalculatorSimpleRoletta();
    testNeedCalculatorInvalidFormulaAudit();
    testNeedCalculatorChooseTrue();
    testNeedCalculatorChooseFalse();

    zInfo("=== FormulaEngine TESTS END ===");
    return true;
}

// ---------------------------------------------------------------------
// Helper
// ---------------------------------------------------------------------

static void setVars(int w, int h, int qty)
{
    auto& vars = VariableRepository::instance();
    vars.clear();
    vars.set("w", Value::numberValue(w));
    vars.set("h", Value::numberValue(h));
    vars.set("qty", Value::numberValue(qty));
}

// ---------------------------------------------------------------------
// A modern DSL több soros scriptet Sequence/Statement AST‑ként futtat
// 1) Alap DSL minták (modern AST‑alapú DSL)
// ---------------------------------------------------------------------

void FormulaEnginePipelineTester::testLiteralInt()
{
    zInfo("→ testLiteralInt");

    setVars(1000, 2000, 1);
    auto r = FormulaEngine::eval("42");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number == 42);

    zInfo("✓ testLiteralInt OK");
}

void FormulaEnginePipelineTester::testLiteralDouble()
{
    zInfo("→ testLiteralDouble");

    setVars(1000, 2000, 1);
    auto r = FormulaEngine::eval("3.14");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT((int)v.number == 3);

    zInfo("✓ testLiteralDouble OK");
}

void FormulaEnginePipelineTester::testSimpleExpression()
{
    zInfo("→ testSimpleExpression");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("w - 10");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number == 1190);

    zInfo("✓ testSimpleExpression OK");
}

void FormulaEnginePipelineTester::testAssignment()
{
    zInfo("→ testAssignment");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("x = w - 15");
    Q_ASSERT(r.ok);

    auto x = VariableRepository::instance().get("x");
    Q_ASSERT(x.number == 1185);

    zInfo("✓ testAssignment OK");
}

void FormulaEnginePipelineTester::testMultiLine()
{
    zInfo("→ testMultiLine");

    setVars(1200, 1500, 1);

    QString script =
        "a = w - 10\n"
        "b = a * 2\n"
        "b";

    auto r = FormulaEngine::eval(script);
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.number == 1190 * 2);

    zInfo("✓ testMultiLine OK");
}

// ---------------------------------------------------------------------
// 2) DSL hibák
// ---------------------------------------------------------------------

void FormulaEnginePipelineTester::testInvalidEmpty()
{
    zInfo("→ testInvalidEmpty");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("");

    Q_ASSERT(!r.ok);

    zInfo("✓ testInvalidEmpty OK");
}

void FormulaEnginePipelineTester::testInvalidGarbage()
{
    zInfo("→ testInvalidGarbage");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("this_is_not_valid");

    Q_ASSERT(!r.ok);

    zInfo("✓ testInvalidGarbage OK");
}

void FormulaEnginePipelineTester::testUndefinedVariable()
{
    zInfo("→ testUndefinedVariable");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("foo + 10");

    Q_ASSERT(!r.ok);

    zInfo("✓ testUndefinedVariable OK");
}

void FormulaEnginePipelineTester::testUndefinedFunction()
{
    zInfo("→ testUndefinedFunction");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("unknownFn(10)");

    Q_ASSERT(!r.ok);

    zInfo("✓ testUndefinedFunction OK");
}

void FormulaEnginePipelineTester::testDivisionByZero()
{
    zInfo("→ testDivisionByZero");

    setVars(1200, 1500, 1);
    auto r = FormulaEngine::eval("div(10,0)");

    Q_ASSERT(!r.ok);

    zInfo("✓ testDivisionByZero OK");
}

void FormulaEnginePipelineTester::testNestedFunctionCall()
{
    zInfo("→ testNestedFunctionCall");

    // nincs szükség w,h,qty változókra
    VariableRepository::instance().clear();

    auto r = FormulaEngine::eval("mul(2, add(3,4))");

    Q_ASSERT(r.ok);

    Value result = VariableRepository::instance().get("_result");

    Q_ASSERT(result.type == Value::Type::Number);
    Q_ASSERT(result.number == 14.0);

    zInfo("✓ testNestedFunctionCall OK");
}

// ---------------------------------------------------------------------
// 3) choose / opt
// ---------------------------------------------------------------------

void FormulaEnginePipelineTester::testChooseSimple()
{
    zInfo("→ testChooseSimple");

    setVars(2000, 2000, 1);
    auto r = FormulaEngine::eval("choose: (w*h > 1000000) ? A : B");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.text == "A");

    zInfo("✓ testChooseSimple OK");
}

void FormulaEnginePipelineTester::testChooseNested()
{
    zInfo("→ testChooseNested");

    setVars(2000, 2000, 1);

    QString script =
        "choose: (w > 1500) ? "
        "choose: (h > 1500) ? X : Y "
        ": Z";

    auto r = FormulaEngine::eval(script);
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.text == "X");

    zInfo("✓ testChooseNested OK");
}

void FormulaEnginePipelineTester::testChooseFalseBranch()
{
    zInfo("→ testChooseFalseBranch");

    setVars(1000, 1000, 1);
    auto r = FormulaEngine::eval("choose: (w*h > 5000000) ? A : B");
    Q_ASSERT(r.ok);

    auto v = VariableRepository::instance().get("_result");
    Q_ASSERT(v.text == "B");

    zInfo("✓ testChooseFalseBranch OK");
}

void FormulaEnginePipelineTester::testOptSimple()
{
    zInfo("→ testOptSimple");

    setVars(1200, 1500, 1);
    auto& vars = VariableRepository::instance();
    vars.set("paint", Value::boolValue(false));

    auto r = FormulaEngine::eval("h - 10 + opt:paint:+40");
    Q_ASSERT(r.ok);

    auto v = vars.get("_result");
    Q_ASSERT(v.number == 1490);

    zInfo("✓ testOptSimple OK");
}

void FormulaEnginePipelineTester::testOptExpression()
{
    zInfo("→ testOptExpression");

    setVars(1200, 1500, 1);
    auto& vars = VariableRepository::instance();
    vars.set("premium", Value::boolValue(true));

    auto r = FormulaEngine::eval("w - 10 + opt:premium:+(w/10)");
    Q_ASSERT(r.ok);

    auto v = vars.get("_result");
    Q_ASSERT(v.number == 1190 + 120);

    zInfo("✓ testOptExpression OK");
}

// ---------------------------------------------------------------------
// 4) NeedCalculator integráció
// ---------------------------------------------------------------------

void FormulaEnginePipelineTester::testNeedCalculatorSimpleRoletta()
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

    auto cuts = NeedCalculator::makeCutList({ ids.P1, 1200, 1500, 1 }, "Manufacturing");

    Q_ASSERT(cuts.size() == 2);
    Q_ASSERT(cuts[0].length_mm == 1185);
    Q_ASSERT(cuts[1].length_mm == 1190);

    zInfo("✓ testNeedCalculatorSimpleRoletta OK");
}

void FormulaEnginePipelineTester::testNeedCalculatorInvalidFormulaAudit()
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
    d.formula = "w-"; // hibás
    Q_ASSERT(!NeedCalculationDetailRegistry::instance().insert(d));

    auto cuts = NeedCalculator::makeCutList({ ids.P1, 1200, 1500, 1 }, "Manufacturing");

    Q_ASSERT(cuts.isEmpty());

    zInfo("✓ testNeedCalculatorInvalidFormulaAudit OK");
}

void FormulaEnginePipelineTester::testNeedCalculatorChooseTrue()
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

    auto cuts = NeedCalculator::makeCutList({ ids.P1, 2000, 2000, 1 }, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialId == ids.M1);

    zInfo("✓ testNeedCalculatorChooseTrue OK");
}

void FormulaEnginePipelineTester::testNeedCalculatorChooseFalse()
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

    auto cuts = NeedCalculator::makeCutList({ ids.P1, 1000, 1000, 1 }, "Manufacturing");

    Q_ASSERT(cuts.size() == 1);
    Q_ASSERT(cuts[0].materialId == ids.M2);

    zInfo("✓ testNeedCalculatorChooseFalse OK");
}
