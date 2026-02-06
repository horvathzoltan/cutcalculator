#pragma once
#include "test/common/test_module_base.h"

class MaterialRegistryTester : public TestModuleBase {
public:
    MaterialRegistryTester()
        : TestModuleBase("MaterialRegistry")
    {}

    bool run() override;

private:
    void writeTestCsv_multi();
    void testLoad_multi();
    void testFindByBarcode();
    void testFindIfMultiple();
    void testPointerStability();
};
