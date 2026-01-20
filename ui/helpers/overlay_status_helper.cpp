#include "overlay_status_helper.h"
#include "ui/widgets/overlay_icon_widget.h"

// ------------------------------------------------------------
// Állapot meghatározása
// ------------------------------------------------------------
OverlayStatusHelper::State
OverlayStatusHelper::computeState(int repoCount, int visibleRows)
{
    if (repoCount == 0)
        return State::EmptyRepo;

    if (visibleRows == 0)
        return State::NoVisibleRows;

    return State::Normal;
}

// ------------------------------------------------------------
// Állapot alkalmazása a widgetre
// ------------------------------------------------------------
void OverlayStatusHelper::apply(OverlayIconWidget* w, State s)
{
    if (!w)
        return;

    // Base emoji minden toronyban egységes
    w->setBaseEmoji("📄");

    switch (s) {
    case State::EmptyRepo:
        w->setOverlay(OverlayIconWidget::BottomRight, "❌");
        break;

    case State::NoVisibleRows:
        w->setOverlay(OverlayIconWidget::BottomRight, "🟡");
        break;

    case State::Normal:
        w->setOverlay(OverlayIconWidget::BottomRight, "🟢");
        break;
    }
}

