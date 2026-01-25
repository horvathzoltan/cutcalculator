#pragma once
#include "test/common/test_module_base.h"
#include "calculation/model/need_calculation_detail.h"
#include "test/common/test_data_builder.h"

class NeedCalculationDetailRegistryTester : public TestModuleBase {
public:
    NeedCalculationDetailRegistryTester()
        : TestModuleBase("NeedCalculationDetailRegistry")
    {}

    bool run() override;

private:
    TestDataIds ids;

    NeedCalculationDetail makeD(const QUuid& calcId,
                                const QUuid& materialId,
                                const QString& formula);
    void prepare();

    void testValidInsert();
    void testDuplicateInsert();
    void testInvalidDomain();
    void testValidUpdate();
    void testInvalidUpdateDuplicate();
    void testValidRemove();
    void testInvalidRemove();
    void testFindByCalculation();
    void testSubscription();
};
