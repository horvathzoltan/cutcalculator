#pragma once
#include "test/common/test_module_base.h"

class MaterialRegistryTester : public TestModuleBase {
public:
    MaterialRegistryTester()
        : TestModuleBase("MaterialRegistry")
    {}

    bool run() override;

private:
    void writeTestCsv();
    void testLoad();
    void testFindById();
    void testBarcodeIndex();
};
