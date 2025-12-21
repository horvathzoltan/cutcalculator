#pragma once

#include <QWidget>
#include <QSplitter>
#include <QScreen>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QHeaderView>

#include "common/logger/event_logger.h"

/**
 * @class GeometryHelper
 * @brief Percent-based window and splitter state save/restore helper.
 *
 * Responsibility:
 * - Save window geometry as percentages of current screen size (x%,y%,w%,h%).
 * - Restore window geometry from percentages, recalculating on current screen.
 * - Save splitter sizes as percentages across its children (e.g., "70%,30%").
 * - Restore splitter sizes from percent strings (horizontal/vertical aware).
 * - Provide screen size serialization helpers and change detection hooks.
 *
 * Design notes:
 * - The helper is stateless, pure static API.
 * - Percent strings are human-readable and audit-friendly (e.g., "10%,10%,80%,80%").
 * - Logging uses zEventINFO/zEventWARN to keep all steps audit-visible.
 * - No direct Settings access: the caller reads/writes strings with SettingsManager.
 */
class GeometryHelper {
public:
    /**
     * Save the current window geometry as percent string relative to its screen size.
     *
     * Format: "x%,y%,w%,h%" with one decimal precision (e.g., "10.0%,10.0%,80.0%,80.0%").
     * If no screen is available, returns an empty string.
     */
    static QString saveWindowGeometry(QWidget* window);

    /**
     * Restore window geometry from percent string and log screen changes.
     *
     * @param window           Target window.
     * @param percentGeometry  "x%,y%,w%,h%" string.
     * @param savedScreenSize  Previous screen size used at save-time, for change logging.
     *
     * Behavior:
     * - If percentGeometry invalid → warns and returns.
     * - If screen changed → logs info and recalculates based on current screen.
     */
    static void restoreWindowGeometry(QWidget* window,
                                      const QString& percentGeometry,
                                      const QSize& savedScreenSize);

    /**
     * Save splitter sizes as percent string (e.g., "70.0%,30.0%").
     *
     * Uses QSplitter::sizes() as the source and normalizes to 100%.
     * If total = 0 (uninitialized), returns empty string.
     */
    static QString saveSplitterState(QSplitter* splitter);

    /**
     * Restore splitter sizes from percent string.
     *
     * @param splitter       Target splitter.
     * @param percentState   "p1%,p2%,..." matching child count. Extra values are ignored;
     *                       missing values are padded by equal distribution of the remainder.
     *
     * Behavior:
     * - Orientation-aware (horizontal/vertical total based on widget size).
     * - If percentState invalid → warns and returns.
     */
    static void restoreSplitterState(QSplitter* splitter, const QString& percentState);

    /**
     * Serialize a screen size to "WxH" (e.g., "1920x1080").
     */
    static QString serializeScreenSize(const QSize& sz);

    /**
     * Parse "WxH" to QSize. If invalid, returns QSize().
     */
    static QSize parseScreenSize(const QString& s);


    static QString saveHeaderState(QHeaderView *header);
    static void restoreHeaderState(QHeaderView *header, const QString &percentState);
    static bool isWindowGeometryReady(QWidget *w);
private:
    static double parsePercentToken(const QString& token);
    static QStringList toPercentTokens(const QList<int>& sizes);
    static QList<int> scalePercentTokens(const QStringList& tokens, int totalPixels, int childCount);
};
