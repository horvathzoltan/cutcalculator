#pragma once
#include "test/common/test_module_base.h"
#include <QString>

class FileNameHelperTester : public TestModuleBase {
public:
    FileNameHelperTester()
        : TestModuleBase("FileNameHelper")
    {}

    bool run() override;

private:
    // tests
    void testBootstrap();
    void testRootSwitching();
    void testMainDataPaths();
    void testRegistryPaths();
    void testFallbackLogic();
    void testDirectoryCreation();
};
