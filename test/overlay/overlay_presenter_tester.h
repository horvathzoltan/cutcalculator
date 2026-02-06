#pragma once

#include "test/common/test_module_base.h"

class OverlayPresenterTester : public TestModuleBase {
public:
    OverlayPresenterTester()
        : TestModuleBase("OverlayPresenter")
    {}

    bool run() override;

private:
    void testEmptyRepoState();
    void testNoVisibleRowsState();
    void testNormalState();
};
