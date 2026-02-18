#include "formula_engine_pipeline_tester.h"

#include "expression/formula_engine.h"
#include "expression/variable.h"
#include "common/logger/logger.h"

bool FormulaEnginePipelineTester::run()
{
    zInfo("=== FormulaEngine PIPELINE TESTS START ===");

    testLiteralInt();
    testLiteralDouble();
    testSimpleExpression();
    testAssignment();
    testMultiLine();
    testChooseSimple();
    testChooseNested();
    testOptSimple();
    testOptExpression();
    testCombined();

    zInfo("=== FormulaEngine PIPELINE TESTS END ===");
    return true;
}

static void setVars(int w, int h, int qty)
{
    auto& vars = VariableRepository::instance();
    vars.clear();
    vars.set("w", w);
    vars.set("h", h);
    vars.set("qty", qty);
}

// ------------------------------------------------------------
// 1) Alapesetek
// ------------------------------------------------------------

void FormulaEnginePipelineTester::testLiteralInt()
{
    zInfo("→ testLiteralInt");

    setVars(1000, 2000, 1);
    auto ev = FormulaEngine::eval("42");

    Q_ASSERT(ev.length_mm == 42);
    Q_ASSERT(ev.pieces == 1);
}

void FormulaEnginePipelineTester::testLiteralDouble()
{
    zInfo("→ testLiteralDouble");

    setVars(1000, 2000, 1);
    auto ev = FormulaEngine::eval("3.14");

    Q_ASSERT(ev.length_mm == 3);
    Q_ASSERT(ev.pieces == 1);
}

void FormulaEnginePipelineTester::testSimpleExpression()
{
    zInfo("→ testSimpleExpression");

    setVars(1200, 1500, 1);
    auto ev = FormulaEngine::eval("w - 10");

    Q_ASSERT(ev.length_mm == 1190);
    Q_ASSERT(ev.pieces == 1);
}

// ------------------------------------------------------------
// 2) Assignment + Multi-line
// ------------------------------------------------------------

void FormulaEnginePipelineTester::testAssignment()
{
    zInfo("→ testAssignment");

    setVars(1200, 1500, 1);
    auto ev = FormulaEngine::eval("x = w - 15");

    Q_ASSERT(ev.length_mm == 1185);
    Q_ASSERT(VariableRepository::instance().get("x").toInt() == 1185);
}

void FormulaEnginePipelineTester::testMultiLine()
{
    zInfo("→ testMultiLine");

    setVars(1200, 1500, 1);

    QString script =
        "a = w - 10\n"
        "b = a * 2\n"
        "b";

    auto ev = FormulaEngine::eval(script);

    Q_ASSERT(ev.length_mm == (1190 * 2));
}

// ------------------------------------------------------------
// 3) choose: DSL
// ------------------------------------------------------------

void FormulaEnginePipelineTester::testChooseSimple()
{
    zInfo("→ testChooseSimple");

    setVars(2000, 2000, 1);
    auto ev = FormulaEngine::eval("choose: (w*h > 1000000) ? A : B");

    Q_ASSERT(ev.stringValue == "A");
}

void FormulaEnginePipelineTester::testChooseNested()
{
    zInfo("→ testChooseNested");

    setVars(2000, 2000, 1);

    QString script =
        "choose: (w > 1500) ? "
        "    choose: (h > 1500) ? X : Y "
        "  : Z";

    auto ev = FormulaEngine::eval(script);

    Q_ASSERT(ev.stringValue == "X");
}

// ------------------------------------------------------------
// 4) opt: DSL
// ------------------------------------------------------------

void FormulaEnginePipelineTester::testOptSimple()
{
    zInfo("→ testOptSimple");

    setVars(1200, 1500, 1);

    auto& vars = VariableRepository::instance();
    vars.set("paint", false);

    auto ev = FormulaEngine::eval("h - 10 + opt:paint:+40");

    Q_ASSERT(ev.length_mm == 1490);
}

void FormulaEnginePipelineTester::testOptExpression()
{
    zInfo("→ testOptExpression");

    setVars(1200, 1500, 1);

    auto& vars = VariableRepository::instance();
    vars.set("premium", true);

    auto ev = FormulaEngine::eval("w - 10 + opt:premium:+(w/10)");

    Q_ASSERT(ev.length_mm == (1190 + 120));
}

// ------------------------------------------------------------
// 5) Kombinált eset
// ------------------------------------------------------------

void FormulaEnginePipelineTester::testCombined()
{
    zInfo("→ testCombined");

    setVars(2000, 2000, 1);

    auto& vars = VariableRepository::instance();
    vars.set("premium", true);

    QString script =
        "base = w - 20\n"
        "opt:premium:+(base/2)\n"
        "choose: base > 1500 ? base + opt : base";

    auto ev = FormulaEngine::eval(script);

    // base = 1980
    // opt = 990
    // choose → true ág → 1980 + 990 = 2970
    Q_ASSERT(ev.length_mm == 2970);
}
