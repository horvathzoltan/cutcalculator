// test/calculation/roletta_calculation_tester.h
#pragma once
#include "test/common/test_module_base.h"
#include "test/calculation/test_roletta_builder.h"

class RolettaCalculationTester : public TestModuleBase {
public:
    RolettaCalculationTester()
        : TestModuleBase("RolettaCalculation")
    {}

    bool run() override;

private:
    void verify_builder(const TestRolettaBuilder::Ids& ids);

    void test_rugos_roletta(const TestRolettaBuilder::Ids& ids);
    void test_tetoteri_roletta(const TestRolettaBuilder::Ids& ids);
    void test_alap_roletta(const TestRolettaBuilder::Ids& ids);
};
