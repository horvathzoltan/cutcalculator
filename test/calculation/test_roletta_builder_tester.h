#pragma once
#include "test/common/test_module_base.h"
#include "test/calculation/test_roletta_builder.h"

class TestRolettaBuilderTester : public TestModuleBase {
public:
    TestRolettaBuilderTester()
        : TestModuleBase("TestRolettaBuilder")
    {}

    bool run() override;
};
