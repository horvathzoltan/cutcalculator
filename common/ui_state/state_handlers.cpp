#include "common/ui_state/state_handlers.h"
#include "common/logger/logger.h"

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
    if (!w) {
        zWarning() << "⚠️ extract: null widget for key" << key;
        return;
    }


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
    if (!w) {
        zWarning() << "⚠️ extract: null widget for key" << key;
        return;
    }

    auto* header = qobject_cast<QHeaderView*>(w);
    if (!header) return;

    QAbstractItemModel* model = header->model();
    if (!model) return;

    const int count = header->count();
    if (count <= 0) return;

    const int viewportWidth = header->viewport()->width();
    m[key + "/viewportWidthPx"] = viewportWidth;

    for (int section = 0; section < count; ++section) {
        const QString colName =
            model->headerData(section, Qt::Horizontal).toString();

        if (colName.isEmpty())
            continue;

        const int px = header->sectionSize(section);
        const double pct = viewportWidth > 0
                               ? (100.0 * px / double(viewportWidth))
                               : 0.0;

        m[key + "/widthPercent_" + colName] = pct;
    }

    m[key + "/sortColumnName"] =
        model->headerData(header->sortIndicatorSection(),
                          Qt::Horizontal).toString();

    m[key + "/sortOrder"] =
        int(header->sortIndicatorOrder());
}

void restoreHeader(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* header = qobject_cast<QHeaderView*>(w);
    if (!header) return;

    QAbstractItemModel* model = header->model();
    if (!model) return;

    const int viewportWidth =
        m.value(key + "/viewportWidthPx", header->viewport()->width()).toInt();

    const int count = header->count();
    for (int section = 0; section < count; ++section) {

        const QString colName =
            model->headerData(section, Qt::Horizontal).toString();

        if (colName.isEmpty())
            continue;

        const QString pctKey = key + "/widthPercent_" + colName;
        if (!m.contains(pctKey))
            continue;

        const double pct = m.value(pctKey).toDouble();
        const int px = int((pct / 100.0) * viewportWidth);

        header->resizeSection(section, px);
    }

    const QString sortColName =
        m.value(key + "/sortColumnName").toString();

    if (!sortColName.isEmpty()) {
        for (int section = 0; section < count; ++section) {
            const QString colName =
                model->headerData(section, Qt::Horizontal).toString();
            if (colName == sortColName) {
                const auto sortOrder =
                    Qt::SortOrder(m.value(key + "/sortOrder",
                                          int(Qt::AscendingOrder)).toInt());
                header->setSortIndicator(section, sortOrder);
                break;
            }
        }
    }
}


// -----------------------------
// QTabWidget
// -----------------------------
void extractTabWidget(QWidget* w, QVariantMap& m, const QString& key)
{
    if (!w) {
        zWarning() << "⚠️ extract: null widget for key" << key;
        return;
    }

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
    if (!w) {
        zWarning() << "⚠️ extract: null widget for key" << key;
        return;
    }

    auto* area = qobject_cast<QAbstractScrollArea*>(w);
    if (!area) return;

    if (auto* h = area->horizontalScrollBar())
        m[key + "/scrollPosX"] = h->value();

    if (auto* v = area->verticalScrollBar())
        m[key + "/scrollPosY"] = v->value();
}

void restoreScrollArea(QWidget* w, QVariantMap& m, const QString& key)
{
    auto* area = qobject_cast<QAbstractScrollArea*>(w);
    if (!area) return;

    if (auto* h = area->horizontalScrollBar())
        h->setValue(m.value(key + "/scrollPosX", h->value()).toInt());

    if (auto* v = area->verticalScrollBar())
        v->setValue(m.value(key + "/scrollPosY", v->value()).toInt());
}

} // namespace StateHandlers
