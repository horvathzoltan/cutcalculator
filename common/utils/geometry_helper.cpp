#include "common/utils/geometry_helper.h"
#include <QHeaderView>
#include <cmath>

/* window */

QString GeometryHelper::saveWindowGeometry(QWidget* window) {
    if (!window || !window->screen()) {
        zEventWARN("⚠️ Window geometry save skipped: no window or screen");
        return {};
    }

    const QSize screenSize = window->screen()->size();
    const QRect g = window->geometry();

    const auto toPct = [&](int val, int base) -> double {
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

    zEventINFO(QString("✅ Window geometry saved: %1").arg(result));
    return result;
}

void GeometryHelper::restoreWindowGeometry(QWidget* window,
                                           const QString& percentGeometry,
                                           const QSize& savedScreenSize)
{
    if (!window || !window->screen()) {
        zEventWARN("⚠️ Window geometry restore skipped: no window or screen");
        return;
    }
    const QStringList parts = percentGeometry.split(',', Qt::SkipEmptyParts);
    if (parts.size() != 4) {
        zEventWARN(QString("⚠️ Invalid window geometry string: '%1'").arg(percentGeometry));
        return;
    }

    const QSize currentScreenSize = window->screen()->size();
    if (currentScreenSize.isValid() && savedScreenSize.isValid() && currentScreenSize != savedScreenSize) {
        zEventINFO(QString("📺 Screen size changed: %1x%2 → %3x%4")
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
    zEventINFO(QString("✅ Window geometry restored: %1,%2,%3,%4")
                   .arg(x).arg(y).arg(w).arg(h));
}

/* splitter */

QString GeometryHelper::saveSplitterState(QSplitter* splitter) {
    if (!splitter) {
        zEventWARN("⚠️ Splitter save skipped: null splitter");
        return {};
    }
    const QList<int> sizes = splitter->sizes();
    if (sizes.isEmpty()) {
        zEventWARN("⚠️ Splitter save skipped: no sizes");
        return {};
    }
    int total = 0;
    for (int s : sizes) total += s;
    if (total <= 0) {
        zEventWARN("⚠️ Splitter save skipped: total size = 0");
        return {};
    }

    const QStringList tokens = toPercentTokens(sizes);
    const QString result = tokens.join(",");
    zEventINFO(QString("✅ Splitter state saved: %1").arg(result));
    return result;
}

void GeometryHelper::restoreSplitterState(QSplitter* splitter, const QString& percentState) {
    if (!splitter) {
        zEventWARN("⚠️ Splitter restore skipped: null splitter");
        return;
    }
    QStringList tokens = percentState.split(',', Qt::SkipEmptyParts);
    const int childCount = splitter->count();
    if (tokens.isEmpty() || childCount <= 0) {
        zEventWARN(QString("⚠️ Invalid splitter state: '%1'").arg(percentState));
        return;
    }

    // Total pixels depend on orientation
    int totalPixels = (splitter->orientation() == Qt::Vertical)
                          ? splitter->size().height()
                          : splitter->size().width();

    if (totalPixels <= 0) {
        // Fallback: distribute equally if size not yet known.
        QList<int> equal(childCount, 100 / childCount);
        splitter->setSizes(equal);
        zEventWARN("⚠️ Splitter total size unknown; applied equal distribution");
        return;
    }

    // Scale tokens to match child count and pixels
    const QList<int> newSizes = scalePercentTokens(tokens, totalPixels, childCount);
    splitter->setSizes(newSizes);

    // Audit log: mutassuk a pixeleket is
    QStringList pixelStrings;
    for (int sz : newSizes) {
        pixelStrings << QString::number(sz) + "px";
    }
    zEventINFO(QString("✅ Splitter state restored: %1 → [%2]")
                   .arg(percentState, pixelStrings.join(",")));
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
    if (!header) return;
    QStringList parts = percentState.split(',', Qt::SkipEmptyParts);

    // Alap: az összes oszlopszélesség összege vagy a parent viewport width
    int total = 0;
    for (int i = 0; i < header->count(); ++i) {
        total += header->sectionSize(i);
    }
    if (total <= 0) total = header->parentWidget() ? header->parentWidget()->width() : header->width();

    for (int i = 0; i < parts.size() && i < header->count(); ++i) {
        QString p = parts[i];
        if (p.endsWith('%')) p.chop(1);
        bool ok = false;
        double pct = p.toDouble(&ok) / 100.0;
        if (!ok) continue;
        int w = static_cast<int>(total * pct);
        header->resizeSection(i, w);
    }
}
