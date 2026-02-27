#pragma once
#include "test/common/test_module_base.h"

class NeedCalculatorPipelineTester : public TestModuleBase {
public:
    NeedCalculatorPipelineTester()
        : TestModuleBase("NeedCalculatorPipeline")
    {}

    bool run() override;

private:
    void testLenDsl();
    void testQtyDsl();
    void testOptDsl();
    void testChooseDsl();
    void testExplodePieces();
    void testAggregation();
    void testKitting();
};
