#pragma once
#include "test/common/test_module_base.h"

class OrderNeedBuilderTester : public TestModuleBase {
public:
    OrderNeedBuilderTester()
        : TestModuleBase("OrderNeedBuilder")
    {}

    bool run() override;

private:
    void testRolettaOrder();
};
