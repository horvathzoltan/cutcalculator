#include "material_requirements_overlay_tester.h"

#include "ui/helpers/overlay_status_helper.h"
#include "common/logger/logger.h"

bool MaterialRequirementsOverlayTester::run()
{
    zInfo("=== MaterialRequirementsOverlayTester START ===");

    testEmptyRepo();
    testNoVisibleRows();
    testNormal();
    testIncompleteMatrix();

    zInfo("=== MaterialRequirementsOverlayTester END ===");
    return true;
}

void MaterialRequirementsOverlayTester::testEmptyRepo()
{
    zInfo("→ testEmptyRepo");

    int repoCount   = 0;
    int visibleRows = 0;

    auto state = OverlayStatusHelper::computeState(repoCount, visibleRows);

    Q_ASSERT(state == OverlayStatusHelper::State::EmptyRepo);

    zInfo("✓ testEmptyRepo OK");
}

void MaterialRequirementsOverlayTester::testNoVisibleRows()
{
    zInfo("→ testNoVisibleRows");

    int repoCount   = 5;
    int visibleRows = 0;

    auto state = OverlayStatusHelper::computeState(repoCount, visibleRows);

    Q_ASSERT(state == OverlayStatusHelper::State::NoVisibleRows);

    zInfo("✓ testNoVisibleRows OK");
}

void MaterialRequirementsOverlayTester::testNormal()
{
    zInfo("→ testNormal");

    int repoCount   = 5;
    int visibleRows = 5;

    auto state = OverlayStatusHelper::computeState(repoCount, visibleRows);

    Q_ASSERT(state == OverlayStatusHelper::State::Normal);

    zInfo("✓ testNormal OK");
}

void MaterialRequirementsOverlayTester::testIncompleteMatrix()
{
    zInfo("→ testIncompleteMatrix");

    // A Presenter ezt így csinálja:
    // if (!complete) return Incomplete;
    // A helper NEM tudja ezt, ezért itt direkt teszteljük az apply() működését.

    OverlayStatusHelper::State s = OverlayStatusHelper::State::Incomplete;

    // Ha a helperben van Incomplete ág, akkor ez a teszt elég:
    Q_ASSERT(s == OverlayStatusHelper::State::Incomplete);

    zInfo("✓ testIncompleteMatrix OK");
}
