#include "overlay_presenter_tester.h"

#include "ui/helpers/overlay_status_helper.h"
#include "common/logger/logger.h"

bool OverlayPresenterTester::run()
{
    zInfo("=== OverlayPresenter TESTS START ===");

    testEmptyRepoState();
    testNoVisibleRowsState();
    testNormalState();

    zInfo("=== OverlayPresenter TESTS END ===");
    return true;
}

void OverlayPresenterTester::testEmptyRepoState()
{
    zInfo("→ testEmptyRepoState");

    int repoCount    = 0;
    int visibleRows  = 0;

    auto state = OverlayStatusHelper::computeState(repoCount, visibleRows);

    Q_ASSERT(state == OverlayStatusHelper::State::EmptyRepo);

    zInfo("✓ testEmptyRepoState OK");
}

void OverlayPresenterTester::testNoVisibleRowsState()
{
    zInfo("→ testNoVisibleRowsState");

    int repoCount    = 5;  // van adat a registryben
    int visibleRows  = 0;  // de a view nem mutat semmit

    auto state = OverlayStatusHelper::computeState(repoCount, visibleRows);

    Q_ASSERT(state == OverlayStatusHelper::State::NoVisibleRows);

    zInfo("✓ testNoVisibleRowsState OK");
}

void OverlayPresenterTester::testNormalState()
{
    zInfo("→ testNormalState");

    int repoCount    = 5;
    int visibleRows  = 5;

    auto state = OverlayStatusHelper::computeState(repoCount, visibleRows);

    Q_ASSERT(state == OverlayStatusHelper::State::Normal);

    zInfo("✓ testNormalState OK");
}
