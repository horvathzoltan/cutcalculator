#pragma once
#include "common/settings/settings_manager.h"
#include "test/common/test_module_base.h"

class SettingsManagerTester : public TestModuleBase {
public:
    SettingsManagerTester()
        : TestModuleBase("SettingsManager")
    {}

    bool run() override;

private:
    void testFallbackInNormalMode();
    void testNoWriteInTestMode();
    void testFallbackActivationOrder();
    void testMissingKeyBehavior();
};
