#pragma once
#include "calcmodes/model/need_calculation.h"
#include "test/common/test_module_base.h"
#include <test/common/test_data_builder.h>

class NeedCalculationRegistryTester : public TestModuleBase {
public:
    NeedCalculationRegistryTester()
        : TestModuleBase("NeedCalculationRegistry")
    {}

    bool run() override;

private:
    // Helpers
    TestDataIds ids;
    NeedCalculation makeNC(const QUuid& productId, const QString& name);
    void prepare();

    // Tests
    void testValidInsert();
    void testDuplicateInsert();
    void testInvalidDomain();
    void testValidUpdate();
    void testInvalidUpdateDuplicate();
    void testValidRemove();
    void testInvalidRemove();
    void testFindByProductAndName();
    void testSubscription();
};
