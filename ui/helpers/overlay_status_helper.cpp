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

    // Presenter dönti el, hogy incomplete-e
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
    w->setBaseEmoji("📄"); // v2: unified document base emoji

    switch (s) {
    case State::EmptyRepo: // v2: empty repo → red ❌
        w->setOverlay(OverlayIconWidget::BottomRight, "❌");
        break;

    case State::NoVisibleRows: // v2: no visible rows → yellow 🟡
        w->setOverlay(OverlayIconWidget::BottomRight, "🟡");
        break;

    case State::Normal: // v2: normal → green 🟢
        w->setOverlay(OverlayIconWidget::BottomRight, "🟢");
        break;

    // case State::Incomplete: // v2: incomplete → blue 🔵
    //     w->setOverlay(OverlayIconWidget::BottomRight, "🔵");
    //     break;
    }
}

