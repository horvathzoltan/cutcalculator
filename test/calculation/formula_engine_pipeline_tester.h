#pragma once
#include "test/common/test_module_base.h"

class FormulaEnginePipelineTester : public TestModuleBase {
public:
    FormulaEnginePipelineTester()
        : TestModuleBase("FormulaEnginePipeline")
    {}

    bool run() override;

private:
    void testLiteralInt();
    void testLiteralDouble();
    void testSimpleExpression();
    void testAssignment();
    void testMultiLine();
    void testChooseSimple();
    void testChooseNested();
    void testOptSimple();
    void testOptExpression();
    void testCombined();
};
