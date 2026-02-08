#pragma once

#include "test/common/test_module_base.h"

class MatrixValidatorTester : public TestModuleBase {
public:
    MatrixValidatorTester()
        : TestModuleBase("MatrixValidator")
    {}

    bool run() override;

private:
    void testEmptyMatrix();
    void testSingleRuleMissingDetail();
    void testSingleRuleComplete();
    void testMultipleRulesComplete();
    void testMultipleRulesIncomplete();

    void clearAll();
};
