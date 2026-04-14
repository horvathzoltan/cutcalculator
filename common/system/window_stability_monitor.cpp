#include "window_stability_monitor.h"

#include <QWidget>
#include <QScreen>
#include <QTabWidget>
#include <QSplitter>

#include "common/system/verbose_manager.h"
#include "common/utils/geometry_helper.h"
#include "common/snapshot/snapshot_manager.h"

WindowStabilityMonitor& WindowStabilityMonitor::instance()
{
    static WindowStabilityMonitor inst;
    return inst;
}

WindowStabilityMonitor::WindowStabilityMonitor(QObject* parent)
    : QObject(parent)
{
    _timer.setInterval(100);
    connect(&_timer, &QTimer::timeout,
            this, &WindowStabilityMonitor::poll);
}

void WindowStabilityMonitor::attachTo(QWidget* window)
{
    _window = window;
    _stableCount = 0;
    _lastProfile.clear();
    _lastDpi = 0.0;
    _lastGeometry = QRect();
    _lastTabsGeometry = QRect();
    _tabsStable = false;
    _splitterStable = false;
    _lastSplitterSizes.clear();

    if (_window && _window->screen()) {
        _lastProfile = SnapshotManager::instance().monitorProfileFor(_window);
        _lastDpi = _window->screen()->logicalDotsPerInch();
        _lastGeometry = _window->geometry();
    }

    _timer.start();
}

bool WindowStabilityMonitor::isStableOnce() const
{
    if (!_window)
        return false;

    if (!_window->isVisible())
        return false;

    // ne fogadjuk el túl kicsinek
    if (_window->width() < 300 || _window->height() < 200)
        return false;

    if (!GeometryHelper::isWindowGeometryReady(_window))
        return false;

    // tabWidget létezzen, legyen benne tab, legyen értelmes mérete
    if (auto* tabs = _window->findChild<QTabWidget*>()) {
        if (tabs->count() == 0)
            return false;
        if (tabs->geometry().width() < 100 || tabs->geometry().height() < 50)
            return false;
    }

    // splitter legyen kiosztva
    if (auto* splitter = _window->findChild<QSplitter*>("splitter")) {
        int sum = 0;
        const auto sizes = splitter->sizes();
        for (int s : sizes) sum += s;
        if (sum < 50)
            return false;
    }

    // DPI stabil?
    if (QScreen* s = _window->screen()) {
        double dpi = s->logicalDotsPerInch();
        if (dpi != _lastDpi)
            return false;
    }

    // monitorprofil stabil?
    const QString profile =
        SnapshotManager::instance().monitorProfileFor(_window);
    if (profile != _lastProfile)
        return false;

    // geometry stabil?
    if (_window->geometry() != _lastGeometry)
        return false;

    // tabWidget geometry stabil?
    if (auto* tabs = _window->findChild<QTabWidget*>()) {
        if (!_lastTabsGeometry.isNull() &&
            tabs->geometry() != _lastTabsGeometry)
            return false;
    }

    // splitter stabil?
    if (auto* splitter = _window->findChild<QSplitter*>("splitter")) {
        const auto sizes = splitter->sizes();
        if (!_lastSplitterSizes.isEmpty() &&
            sizes != _lastSplitterSizes)
            return false;
    }

    return true;
}

void WindowStabilityMonitor::poll()
{
    if (!_window) {
        _timer.stop();
        return;
    }

    // --- Diagnosztikai log ---
    if (IS_VERBOSE_THIS()){
    zInfo(QString("🔍 Poll: stableCount=%1 | winGeom=%2x%3 | tabsGeom=%4x%5 | splitterSizes=%6")
              .arg(_stableCount)
              .arg(_window->width())
              .arg(_window->height())
              .arg(_lastTabsGeometry.width())
              .arg(_lastTabsGeometry.height())
              .arg(_lastSplitterSizes.isEmpty() ? -1 : _lastSplitterSizes.first()));
    }

    bool unstable = false;
    bool dpiChanged = false;
    bool profileChanged = false;
    bool geomChanged = false;


    // --- DPI változás ---
    if (QScreen* s = _window->screen()) {
        double dpi = s->logicalDotsPerInch();
        if (dpi != _lastDpi) {
            zInfo(QString("↩️ Instability: DPI changed %1 → %2")
                      .arg(_lastDpi).arg(dpi));
            unstable = true;
            dpiChanged = true;
        }

        _lastDpi = dpi;
    }

    // --- Monitor profil változás ---
    QString profile = SnapshotManager::instance().monitorProfileFor(_window);
    if (profile != _lastProfile) {
        zInfo(QString("↩️ Instability: monitor profile changed '%1' → '%2'")
                  .arg(_lastProfile, profile));
        unstable = true;
        profileChanged = true;
    }


    _lastProfile = profile;

    // --- Window geometry változás ---
    QRect geom = _window->geometry();
    if (geom != _lastGeometry) {
        zInfo(QString("↩️ Instability: window geometry changed (%1,%2 %3x%4) → (%5,%6 %7x%8)")
                  .arg(_lastGeometry.x()).arg(_lastGeometry.y())
                  .arg(_lastGeometry.width()).arg(_lastGeometry.height())
                  .arg(geom.x()).arg(geom.y())
                  .arg(geom.width()).arg(geom.height()));
        unstable = true;
        geomChanged = true;
    }

    _lastGeometry = geom;

    // --- TabWidget geometry változás ---
    if (auto* tabs = _window->findChild<QTabWidget*>()) {
        QRect tg = tabs->geometry();
        if (!_tabsStable && !_lastTabsGeometry.isNull() && tg != _lastTabsGeometry) {
            zInfo(QString("↩️ Instability: tabWidget geometry changed (%1x%2) → (%3x%4)")
                      .arg(_lastTabsGeometry.width()).arg(_lastTabsGeometry.height())
                      .arg(tg.width()).arg(tg.height()));
            unstable = true;
        }
        _lastTabsGeometry = tg;
        if (!_tabsStable && !tg.isNull() && !unstable) {
            _tabsStable = true;
        }
    }


    // --- Splitter méret változás ---
    if (auto* splitter = _window->findChild<QSplitter*>("splitter")) {
        const auto sizes = splitter->sizes();
        int sum = 0;
        for (int s : sizes) sum += s;

        if (!_splitterStable && !_lastSplitterSizes.isEmpty() && sizes != _lastSplitterSizes && sum >= 50) {
            zInfo(QString("↩️ Instability: splitter sizes changed %1 → %2")
                      .arg(QString::fromLatin1(QByteArray::number(_lastSplitterSizes.first())))
                      .arg(QString::fromLatin1(QByteArray::number(sizes.first()))));
            unstable = true;
        }
        _lastSplitterSizes = sizes;

        if (!_splitterStable && sum >= 50 && !unstable) {
            _splitterStable = true;
        }
    }


    // --- Ha bármelyik instabil volt, nullázzuk ---
    if (unstable) {
        _stableCount = 0;
    }

    // --- Stabilitási feltételek ellenőrzése ---
    if (isStableOnce()) {
        _stableCount++;

        int threshold = 4;
        if (dpiChanged || profileChanged) {
            threshold = 6;
        } else if (geomChanged) {
            threshold = 5;
        } else {
            threshold = 3;
        }

        if (IS_VERBOSE_THIS()){
            zInfo(QString("✔️ Stable cycle %1/%2").arg(_stableCount).arg(threshold));
        }

        if (_stableCount >= threshold) {
            zInfo("🎉 Window is fully stable → emitting windowStable()");
            _timer.stop();
            emit windowStable();
        }
    } else {
        if (!unstable) {
            if (IS_VERBOSE_THIS()){
                zInfo("⚠️ isStableOnce() returned false (structural instability)");
            }
        }
        _stableCount = 0;
    }

}

