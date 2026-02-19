#pragma once
#include "test/common/test_module_base.h"

class FormulaEnginePipelineTester : public TestModuleBase {
public:
    FormulaEnginePipelineTester()
        : TestModuleBase("FormulaEnginePipeline")
    {}

    bool run() override;

private:
    // --- 1) Alap DSL ---
    void testLiteralInt();
    void testLiteralDouble();
    void testSimpleExpression();
    void testAssignment();
    void testMultiLine();

    // --- 2) DSL hibák ---
    void testInvalidEmpty();
    void testInvalidGarbage();
    void testUndefinedVariable();
    void testUndefinedFunction();
    void testDivisionByZero();

    // --- 3) choose / opt DSL ---
    void testChooseSimple();
    void testChooseNested();
    void testChooseFalseBranch();
    void testOptSimple();
    void testOptExpression();

    // --- 4) NeedCalculator integráció ---
    void testNeedCalculatorSimpleRoletta();
    void testNeedCalculatorInvalidFormulaAudit();
    void testNeedCalculatorChooseTrue();
    void testNeedCalculatorChooseFalse();
};
