#include "common/utils/geometry_helper.h"
#include "common/logger/logger.h"
#include <QHeaderView>
#include <cmath>

#include <QTimer>
#include <numeric> // std::accumulate

// Belső segédfüggvény: eldönti, hogy a widget geometry-je már "használható"-e.
static bool isGeometryReadyForRestore(QWidget* w) {
    if (!w) return false;

    // Ha még nem látható, jó eséllyel a layout sem futott végig.
    if (!w->isVisible()) {
        return false;
    }

    // A polish flag azt jelzi, hogy a Qt már lefuttatta a belső init lépéseket.
    if (!w->testAttribute(Qt::WA_WState_Polished)) {
        return false;
    }

    // Hard guard: ha túl kicsi a méret, akkor a layout még nem osztotta ki
    // a valós helyet (klasszikus 0px/15px jelenség).
    const int wWidth  = w->width();
    const int wHeight = w->height();
    if (wWidth < 50 && wHeight < 50) {
        return false;
    }

    return true;
}

bool GeometryHelper::isWindowGeometryReady(QWidget* w) {
    if (!w) return false;

    if (!w->isVisible())
        return false;

    if (!w->testAttribute(Qt::WA_WState_Polished))
        return false;

    if (w->width() < 50 && w->height() < 50)
        return false;

    return true;
}

/* window */
QString GeometryHelper::saveWindowGeometry(QWidget* window) {
    if (!window) {
        zWarning("⚠️ Window geometry save skipped: null window");
        return {};
    }

    // Ha nincs screen, nincs értelme menteni
    if (!window->screen()) {
        zWarning("⚠️ Window geometry save skipped: no screen");
        return {};
    }

    // Guard: ha a window még nem polished vagy nem visible,
    // akkor a geometry valószínűleg nem végleges.
    if (!window->isVisible() ||
        !window->testAttribute(Qt::WA_WState_Polished)) {

        const QSize sz = window->size();
        zWarning(QString("⚠️ Window geometry not stable "
                           "(visible=%1, polished=%2, size=%3x%4) → save postponed")
                       .arg(window->isVisible())
                       .arg(window->testAttribute(Qt::WA_WState_Polished))
                       .arg(sz.width())
                       .arg(sz.height()));

        // Deferred save – újrapróbáljuk az event queue-ban
        QTimer::singleShot(0, window, [window]() {
            GeometryHelper::saveWindowGeometry(window);
        });
        return {};
    }

    const QSize screenSize = window->screen()->size();
    if (!screenSize.isValid()) {
        zWarning("⚠️ Window geometry save skipped: invalid screen size");
        return {};
    }

    const QRect g = window->geometry();

    auto toPct = [&](int val, int base) -> double {
        if (base <= 0) return 0.0;
        return static_cast<double>(val) / static_cast<double>(base) * 100.0;
    };

    const double xPct = toPct(g.x(),      screenSize.width());
    const double yPct = toPct(g.y(),      screenSize.height());
    const double wPct = toPct(g.width(),  screenSize.width());
    const double hPct = toPct(g.height(), screenSize.height());

    const QString result = QString("%1%,%2%,%3%,%4%")
                               .arg(xPct, 0, 'f', 1)
                               .arg(yPct, 0, 'f', 1)
                               .arg(wPct, 0, 'f', 1)
                               .arg(hPct, 0, 'f', 1);

    zInfo(QString("✅ Window geometry saved: %1").arg(result));
    return result;
}



void GeometryHelper::restoreWindowGeometry(QWidget* window,
                                           const QString& percentGeometry,
                                           const QSize& savedScreenSize)
{
    if (!window) {
        zWarning("⚠️ Window geometry restore skipped: null window");
        return;
    }

    const QStringList parts = percentGeometry.split(',', Qt::SkipEmptyParts);
    if (parts.size() != 4) {
        zWarning(QString("⚠️ Invalid window geometry string: '%1'").arg(percentGeometry));
        return;
    }

    // Geometry readiness guard
    if (!isGeometryReadyForRestore(window)) {
        const QSize sz = window->size();
        zWarning(QString("⚠️ Window geometry not ready "
                           "(visible=%1, polished=%2, size=%3x%4) → restore postponed")
                       .arg(window->isVisible())
                       .arg(window->testAttribute(Qt::WA_WState_Polished))
                       .arg(sz.width())
                       .arg(sz.height()));

        QTimer::singleShot(0, window, [window, percentGeometry, savedScreenSize]() {
            GeometryHelper::restoreWindowGeometry(window, percentGeometry, savedScreenSize);
        });
        return;
    }

    if (!window->screen()) {
        zWarning("⚠️ Window geometry restore skipped: no screen");
        return;
    }

    const QSize currentScreenSize = window->screen()->size();
    if (!currentScreenSize.isValid()) {
        zWarning("⚠️ Window geometry restore skipped: invalid screen size");
        return;
    }

    if (savedScreenSize.isValid() && currentScreenSize != savedScreenSize) {
        zInfo(QString("📺 Screen size changed: %1x%2 → %3x%4")
                       .arg(savedScreenSize.width()).arg(savedScreenSize.height())
                       .arg(currentScreenSize.width()).arg(currentScreenSize.height()));
    }

    const auto xPct = parsePercentToken(parts[0]);
    const auto yPct = parsePercentToken(parts[1]);
    const auto wPct = parsePercentToken(parts[2]);
    const auto hPct = parsePercentToken(parts[3]);

    const auto calc = [&](double pct, int base) -> int {
        return std::lround(pct * static_cast<double>(base));
    };

    const int x = calc(xPct, currentScreenSize.width());
    const int y = calc(yPct, currentScreenSize.height());
    const int w = calc(wPct, currentScreenSize.width());
    const int h = calc(hPct, currentScreenSize.height());

    window->setGeometry(x, y, w, h);

    zInfo(QString("✅ Window geometry restored: %1,%2,%3,%4 (screen=%5x%6)")
                   .arg(x).arg(y).arg(w).arg(h)
                   .arg(currentScreenSize.width())
                   .arg(currentScreenSize.height()));
}



/* splitter */

QString GeometryHelper::saveSplitterState(QSplitter* splitter) {
    if (!splitter) {
        zWarning("⚠️ Splitter save skipped: null splitter");
        return {};
    }
    const QList<int> sizes = splitter->sizes();
    if (sizes.isEmpty()) {
        zWarning("⚠️ Splitter save skipped: no sizes");
        return {};
    }
    int total = 0;
    for (int s : sizes) total += s;
    if (total <= 0) {
        zWarning("⚠️ Splitter save skipped: total size = 0");
        return {};
    }

    const QStringList tokens = toPercentTokens(sizes);
    const QString result = tokens.join(",");
    zInfo(QString("✅ Splitter state saved: %1").arg(result));
    return result;
}

void GeometryHelper::restoreSplitterState(QSplitter* splitter, const QString& percentState) {
    if (!splitter) {
        zWarning("⚠️ Splitter restore skipped: null splitter");
        return;
    }

    QStringList tokens = percentState.split(',', Qt::SkipEmptyParts);
    const int childCount = splitter->count();
    if (tokens.isEmpty() || childCount <= 0) {
        zWarning(QString("⚠️ Invalid splitter state: '%1'").arg(percentState));
        return;
    }

    // Geometry readiness guard
    if (!isGeometryReadyForRestore(splitter)) {
        const QSize sz = splitter->size();
        zWarning(QString("⚠️ Splitter geometry not ready "
                           "(visible=%1, polished=%2, size=%3x%4) → restore postponed")
                       .arg(splitter->isVisible())
                       .arg(splitter->testAttribute(Qt::WA_WState_Polished))
                       .arg(sz.width())
                       .arg(sz.height()));

        QTimer::singleShot(0, splitter, [splitter, percentState]() {
            GeometryHelper::restoreSplitterState(splitter, percentState);
        });
        return;
    }

    int totalPixels = (splitter->orientation() == Qt::Vertical)
                          ? splitter->size().height()
                          : splitter->size().width();

    if (totalPixels <= 0) {
        QList<int> equal(childCount, 100 / childCount);
        splitter->setSizes(equal);
        zWarning("⚠️ Splitter total size unknown; applied equal distribution fallback");
        return;
    }

    const QList<int> newSizes = scalePercentTokens(tokens, totalPixels, childCount);
    splitter->setSizes(newSizes);

    QStringList pixelStrings;
    for (int szVal : newSizes) {
        pixelStrings << QString::number(szVal) + "px";
    }
    zInfo(QString("✅ Splitter state restored: %1 → [%2] "
                       "(totalPixels=%3, children=%4)")
                   .arg(percentState, pixelStrings.join(","))
                   .arg(totalPixels)
                   .arg(childCount));
}


/* screen helpers */

QString GeometryHelper::serializeScreenSize(const QSize& sz) {
    if (!sz.isValid()) return {};
    return QString("%1x%2").arg(sz.width()).arg(sz.height());
}

QSize GeometryHelper::parseScreenSize(const QString& s) {
    const auto parts = s.split('x', Qt::SkipEmptyParts);
    if (parts.size() != 2) return {};
    bool okW = false, okH = false;
    const int w = parts[0].toInt(&okW);
    const int h = parts[1].toInt(&okH);
    if (!okW || !okH || w <= 0 || h <= 0) return {};
    return QSize(w, h);
}

/* internals */

double GeometryHelper::parsePercentToken(const QString& token) {
    QString t = token.trimmed();
    if (t.endsWith('%')) t.chop(1);
    bool ok = false;
    const double v = t.toDouble(&ok);
    if (!ok) return 0.0;
    return v / 100.0;
}

QStringList GeometryHelper::toPercentTokens(const QList<int>& sizes) {
    int total = 0;
    for (int s : sizes) total += s;
    QStringList tokens;
    for (int s : sizes) {
        const double pct = total > 0 ? static_cast<double>(s) / static_cast<double>(total) * 100.0 : 0.0;
        tokens << QString::number(pct, 'f', 1) + "%";
    }
    return tokens;
}

QList<int> GeometryHelper::scalePercentTokens(const QStringList& tokens, int totalPixels, int childCount) {
    QList<double> pcts;
    pcts.reserve(tokens.size());
    for (const auto& tok : tokens) {
        pcts << parsePercentToken(tok);
    }

    // Normalize length to childCount: trim extras or pad with equal share
    if (pcts.size() > childCount) {
        pcts = pcts.mid(0, childCount);
    } else if (pcts.size() < childCount) {
        const int missing = childCount - pcts.size();
        const double remain = 1.0 - std::accumulate(pcts.begin(), pcts.end(), 0.0);
        const double pad = (missing > 0) ? remain / static_cast<double>(missing) : 0.0;
        for (int i = 0; i < missing; ++i) pcts << pad;
    }

    // Scale to pixels
    QList<int> sizes;
    sizes.reserve(childCount);
    int sum = 0;
    for (int i = 0; i < childCount; ++i) {
        const int px = std::lround(pcts[i] * static_cast<double>(totalPixels));
        sizes << px;
        sum += px;
    }
    // Correct rounding drift to match totalPixels
    const int drift = totalPixels - sum;
    if (drift != 0 && !sizes.isEmpty()) {
        sizes[0] += drift;
    }
    return sizes;
}


QString GeometryHelper::saveHeaderState(QHeaderView* header) {
    if (!header) return {};
    int total = 0;
    for (int i = 0; i < header->count(); ++i) {
        total += header->sectionSize(i);
    }
    if (total <= 0) return {};

    QStringList percents;
    for (int i = 0; i < header->count(); ++i) {
        int w = header->sectionSize(i);
        double pct = (double)w / total * 100.0;
        percents << QString::number(pct, 'f', 1) + "%";
    }
    return percents.join(",");
}

void GeometryHelper::restoreHeaderState(QHeaderView* header, const QString& percentState) {
    if (!header) {
        zWarning("⚠️ Header restore skipped: null header");
        return;
    }

    QStringList parts = percentState.split(',', Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        zWarning(QString("⚠️ Invalid header percent string: '%1'").arg(percentState));
        return;
    }

    QWidget* parent = header->parentWidget();

    auto isReady = [&]() -> bool {
        if (!header->isVisible())
            return false;
        if (!header->testAttribute(Qt::WA_WState_Polished))
            return false;

        int w = parent ? parent->width() : header->width();
        return w >= 50;
    };

    if (!isReady()) {
        int pw = parent ? parent->width() : header->width();
        zWarning(QString("⚠️ Header geometry not ready "
                           "(visible=%1, polished=%2, parentWidth=%3) → restore postponed")
                       .arg(header->isVisible())
                       .arg(header->testAttribute(Qt::WA_WState_Polished))
                       .arg(pw));

        QTimer::singleShot(0, header, [header, percentState]() {
            GeometryHelper::restoreHeaderState(header, percentState);
        });
        return;
    }

    int total = 0;
    for (int i = 0; i < header->count(); ++i) {
        total += header->sectionSize(i);
    }

    if (total <= 0) {
        total = parent ? parent->width() : header->width();
        if (total <= 0) {
            zWarning("⚠️ Header restore: total width still 0 → postponed again");
            QTimer::singleShot(0, header, [header, percentState]() {
                GeometryHelper::restoreHeaderState(header, percentState);
            });
            return;
        }
    }

    QList<int> newSizes;
    newSizes.reserve(header->count());

    for (int i = 0; i < parts.size() && i < header->count(); ++i) {
        QString p = parts[i].trimmed();
        if (p.endsWith('%'))
            p.chop(1);

        bool ok = false;
        double pct = p.toDouble(&ok) / 100.0;
        if (!ok) {
            zWarning(QString("⚠️ Invalid header percent token: '%1'").arg(parts[i]));
            continue;
        }

        int px = static_cast<int>(std::lround(pct * total));
        newSizes << px;
        header->resizeSection(i, px);
    }

    // ✅ Clazy-clean: nincs range-loop detach
    QStringList pxTokens;
    pxTokens.reserve(newSizes.size());
    for (int i = 0; i < newSizes.size(); ++i) {
        pxTokens << QString::number(newSizes[i]) + "px";
    }

    // ✅ Clazy-clean: multi-arg .arg() használat
    zInfo(QString("✅ Header state restored: %1 → [%2] (total=%3)")
                   .arg(percentState,
                        pxTokens.join(","),
                        QString::number(total)));
}
