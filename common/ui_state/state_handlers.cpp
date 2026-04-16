#include "common/ui_state/state_handlers.h"

#include <QSplitter>
#include <QHeaderView>
#include <QTabWidget>
#include <QAbstractScrollArea>
#include <QScrollBar>

namespace StateHandlers
{

// -----------------------------
// QSplitter
// -----------------------------
void extractSplitter(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* splitter = qobject_cast<QSplitter*>(w);
    if (!splitter) return;

    const auto sizes = splitter->sizes();
    if (sizes.isEmpty()) return;

    int sum = 0;
    for (int v : sizes) sum += v;
    if (sum <= 0) return;

    QVariantList percents;
    for (int v : sizes)
        percents << double(v) / double(sum);

    m[key + "/sizes"] = percents;
}

void restoreSplitter(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* splitter = qobject_cast<QSplitter*>(w);
    if (!splitter) return;

    const auto percents = m.value(key + "/sizes").toList();

    if (percents.isEmpty()) return;

    const int total = splitter->orientation() == Qt::Horizontal
                          ? splitter->width()
                          : splitter->height();

    QList<int> sizes;
    for (const auto& v : percents)
        sizes << int(v.toDouble() * total);

    splitter->setSizes(sizes);
}


// -----------------------------
// QHeaderView
// -----------------------------
void extractHeader(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* header = qobject_cast<QHeaderView*>(w);
    if (!header) return;

    const int count = header->count();
    if (count <= 0) return;

    QVariantList widths;
    for (int i = 0; i < count; ++i)
        widths << header->sectionSize(i);

    m[key + "/widths"] = widths;
    m[key + "/sortSection"] = header->sortIndicatorSection();
    m[key + "/sortOrder"] = int(header->sortIndicatorOrder());

}

void restoreHeader(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* header = qobject_cast<QHeaderView*>(w);
    if (!header) return;

    const auto widths = m.value(key + "/widths").toList();

    if (!widths.isEmpty()) {
        const int count = std::min<int>(header->count(), int(widths.size()));
        for (int i = 0; i < count; ++i)
            header->resizeSection(i, widths[i].toInt());
    }

    const int sortSection = m.value(key + "/sortSection", -1).toInt();
    const auto sortOrder = Qt::SortOrder(m.value(key + "/sortOrder", int(Qt::AscendingOrder)).toInt());
    if (sortSection >= 0 && sortSection < header->count())
        header->setSortIndicator(sortSection, sortOrder);
}


// -----------------------------
// QTabWidget
// -----------------------------
void extractTabWidget(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* tabs = qobject_cast<QTabWidget*>(w);
    if (!tabs) return;

    m[key + "/currentIndex"] = tabs->currentIndex();
}

void restoreTabWidget(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* tabs = qobject_cast<QTabWidget*>(w);
    if (!tabs) return;

    int idx = m.value(key + "/currentIndex", -1).toInt();
    if (idx >= 0 && idx < tabs->count())
        tabs->setCurrentIndex(idx);
}


// -----------------------------
// QAbstractScrollArea
// -----------------------------
void extractScrollArea(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* area = qobject_cast<QAbstractScrollArea*>(w);
    if (!area) return;

    if (auto* h = area->horizontalScrollBar())
        m[key + "/hScroll"] = h->value();

    if (auto* v = area->verticalScrollBar())
        m[key + "/vScroll"] = v->value();
}

void restoreScrollArea(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* area = qobject_cast<QAbstractScrollArea*>(w);
    if (!area) return;

    if (auto* h = area->horizontalScrollBar())
        h->setValue(m.value(key + "/hScroll", h->value()).toInt());

    if (auto* v = area->verticalScrollBar())
        v->setValue(m.value(key + "/vScroll", v->value()).toInt());
}

} // namespace StateHandlers
