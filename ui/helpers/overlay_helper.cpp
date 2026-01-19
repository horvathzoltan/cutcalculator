#include "overlay_helper.h"
#include "ui/widgets/overlay_icon_widget.h"

#include <QWidget>
#include <QHBoxLayout>

OverlayHelper::OverlayHelper(QObject* parent)
    : QObject(parent)
{
}

OverlayIconWidget* OverlayHelper::createStatusWidget(const QString& baseEmoji)
{
    auto* w = new OverlayIconWidget;
    w->setBaseEmoji(baseEmoji);
    return w;
}

void OverlayHelper::attachToView(QWidget* view, OverlayIconWidget* status)
{
    if (!view || !status)
        return;

    // Ha nincs layout, létrehozunk egyet
    if (!view->layout()) {
        auto* lay = new QHBoxLayout(view);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(0);
        view->setLayout(lay);
    }

    auto* lay = qobject_cast<QHBoxLayout*>(view->layout());
    if (!lay) {
        // Ha nem HBoxLayout, akkor is működjön
        auto* h = new QHBoxLayout;
        h->setContentsMargins(0, 0, 0, 0);
        h->setSpacing(0);

        // régi layout tartalmát átemeljük
        while (view->layout()->count() > 0) {
            QLayoutItem* it = view->layout()->takeAt(0);
            h->addItem(it);
        }

        delete view->layout();
        view->setLayout(h);
        lay = h;
    }

    // Spacer → jobbra tolja a status widgetet
    lay->addStretch();
    lay->addWidget(status);
}


void OverlayHelper::update(OverlayIconWidget* status,
                           int repoCount,
                           int visibleRows)
{
    if (!status)
        return;

    // A widget saját, bevált logikáját használjuk:
    status->updateOverlayState2(repoCount, visibleRows);
}

void OverlayHelper::warning(OverlayIconWidget* status)
{
    if (!status)
        return;

    status->setBaseEmoji("📄");
    status->setOverlay(OverlayIconWidget::BottomRight, "🟡");
}

void OverlayHelper::error(OverlayIconWidget* status)
{
    if (!status)
        return;

    status->setBaseEmoji("📄");
    status->setOverlay(OverlayIconWidget::BottomRight, "❌");
}
