#pragma once

#include "test/common/test_module_base.h"

class MaterialRequirementsOverlayTester : public TestModuleBase {
public:
    MaterialRequirementsOverlayTester()
        : TestModuleBase("MaterialRequirementsOverlay")
    {}

    bool run() override;

private:
    void testEmptyRepo();
    void testNoVisibleRows();
    void testNormal();
    void testIncompleteMatrix();
};
