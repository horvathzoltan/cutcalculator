#pragma once
#include "test/common/test_module_base.h"

class EventLoggerTester : public TestModuleBase {
public:
    EventLoggerTester()
        : TestModuleBase("EventLogger")
    {}

    bool run() override;

private:
    void testParseLine();
    void testColorForPrefix();
};
