#pragma once

#include "test/common/test_module_base.h"

class FormulaEngineTester : public TestModuleBase {
public:
    FormulaEngineTester()
        : TestModuleBase("FormulaEngine")
    {}

    bool run() override;

private:
    // Alap DSL minták
    void testWidthMinus();
    void testHeightMinus();
    void testFixedPieces();
    void testAreaLike();
    void testInvalidEmpty();
    void testInvalidGarbage();
    void testChooseSimple();
    void testChooseFalseBranch();
    void testOptSimple();
    void testNeedCalculatorChooseTrue();
    void testNeedCalculatorChooseFalse();

    // Integráció NeedCalculator-ral (OrderLine → NeedResult)
    void testNeedCalculatorSimpleRoletta();
    void testNeedCalculatorInvalidFormulaAudit();
};
