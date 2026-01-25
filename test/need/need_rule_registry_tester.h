#pragma once
#include "test/common/test_module_base.h"
#include "needs/model/need_rule.h"
#include "test/common/test_data_builder.h"

class NeedRuleRegistryTester : public TestModuleBase {
public:
    NeedRuleRegistryTester()
        : TestModuleBase("NeedRuleRegistry")
    {}

    bool run() override;

private:
    TestDataIds ids;

    void prepare();
    void clearAllRegistries();

    void testValidInsert();
    void testDuplicateInsert();
    void testInvalidLeft();
    void testInvalidRight();
    void testValidRemove();
    void testInvalidRemove();
    void testFindByLeft();
    void testRightConvenience();
    void testSubscription();
};
