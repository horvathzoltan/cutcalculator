#pragma once

#include "test/common/test_module_base.h"

class FormulaEngineTester : public TestModuleBase {
public:
    FormulaEngineTester()
        : TestModuleBase("FormulaEngine")
    {}

    bool run() override;

    void testOptSimpleTrue();
private:
    static void setVars(int w, int h, int qty);
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
