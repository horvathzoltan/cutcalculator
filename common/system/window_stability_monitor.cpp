#include "window_stability_monitor.h"

// #include <QWidget>
// #include <QScreen>
// #include <QTabWidget>
// #include <QSplitter>
// #include <QMenuBar>
// #include <QToolBar>
// #include <QStatusBar>
// #include <QAbstractButton>
// #include <QLabel>
// #include <QComboBox>
// #include <QScrollBar>
// #include <QLineEdit>

#include "common/logger/logger.h"
#include "common/system/verbose_manager.h"
#include "common/utils/window_geometry_helper.h"

#include <common/ui_state/widget_discovery_helper.h>
#include <common/window_state/window_state_manager.h>

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
    _lastWidgetState.clear();

    _window = window;
    _stableCount = 0;
    _lastProfile.clear();
    _lastDpi = 0.0;
    _lastGeometry = QRect();
    _lastTabsGeometry = QRect();
    _lastSplitterSizes.clear();
    _tabsStableCount = 0;
    _splitterStableCount = 0;
    _childrenStableCount = 0;
    _recentInstabilityCount = 0;
    _restoreTriggered = false;

    if (_window && _window->screen()) {
        _lastProfile = WindowStateManager::instance().monitorProfileFor(_window);
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

    if (!WindowGeometryHelper::isWindowGeometryReady(_window))
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
        WindowStateManager::instance().monitorProfileFor(_window);
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

    QRect tabsGeom;
    QList<int> splitterSizes;

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
    QString profile = WindowStateManager::instance().monitorProfileFor(_window);
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
        //geomChanged = true;
    }

    // --- TabWidget geometry változás ---
    if (auto* tabs = _window->findChild<QTabWidget*>()) {
        tabsGeom = tabs->geometry();

        if (!_lastTabsGeometry.isNull() && tabsGeom != _lastTabsGeometry) {
            zInfo(QString("↩️ Instability: tabWidget geometry changed (%1x%2) → (%3x%4)")
                      .arg(_lastTabsGeometry.width()).arg(_lastTabsGeometry.height())
                      .arg(tabsGeom.width()).arg(tabsGeom.height()));
            unstable = true;
        }

        if (!tabsGeom.isNull() && !unstable) {
            if (tabsGeom == _lastTabsGeometry)
                _tabsStableCount++;
            else
                _tabsStableCount = 0;
        }
    }

    // --- Splitter méret változás ---
    if (auto* splitter = _window->findChild<QSplitter*>("splitter")) {
        splitterSizes = splitter->sizes();

        int sum = 0;
        for (int i = 0; i < splitterSizes.size(); ++i) {
            sum += splitterSizes[i];
        }

        if (!_lastSplitterSizes.isEmpty() && splitterSizes != _lastSplitterSizes && sum >= 50) {
            zInfo(QString("↩️ Instability: splitter sizes changed %1 → %2")
                      .arg(QString::fromLatin1(QByteArray::number(_lastSplitterSizes.first())))
                      .arg(QString::fromLatin1(QByteArray::number(splitterSizes.first()))));
            unstable = true;
        }

        if (sum >= 50 && !unstable) {
            if (splitterSizes == _lastSplitterSizes)
                _splitterStableCount++;
            else
                _splitterStableCount = 0;
        }
    }



    // --- Ha bármelyik instabil volt, nullázzuk ---
    if (unstable) {
        _stableCount = 0;
        _recentInstabilityCount++;
        if (_recentInstabilityCount > 10)
            _recentInstabilityCount = 10;
    } else {
        if (_recentInstabilityCount > 0)
            _recentInstabilityCount--;
    }

    QList<QWidget*> widgets = WidgetDiscoveryHelper::collect(_window);
    int widgetCount = widgets.size();

    bool childrenStable = areWidgetsStable(widgets);
    if (childrenStable)
        _childrenStableCount++;
    else
        _childrenStableCount = 0;

    // --- Stabilitási feltételek ellenőrzése ---
    if (isStableOnce() &&
        _tabsStableCount >= 1 &&
        _splitterStableCount >= 1 &&
        _childrenStableCount >= 1) {

        bool dpiChangedNow = dpiChanged;
        bool profileChangedNow = profileChanged;

        // --- Sanity-check: ha az elmúlt 2 ciklusban volt instabilitás, halasszuk ---
        if (_recentInstabilityCount > 0) {
            _stableCount = 0;
            return;
        }

        _stableCount++;

        int threshold = computeAdaptiveThreshold(
            widgetCount,
            dpiChangedNow,
            profileChangedNow,
            _recentInstabilityCount
            );

        if (IS_VERBOSE_THIS()) {
            zInfo(QString("✔️ Stable cycle %1/%2 (widgets=%3, recentInst=%4, dpi=%5, profile=%6)")
                      .arg(_stableCount)
                      .arg(threshold)
                      .arg(widgetCount)
                      .arg(_recentInstabilityCount)
                      .arg(dpiChangedNow)
                      .arg(profileChangedNow));
        }

        if (_stableCount >= threshold) {
            if (!_restoreTriggered) {
                _restoreTriggered = true;
                zInfo("🎉 Window is fully stable → emitting windowStable()");
                _timer.stop();
                emit windowStable();
                emit uiReadyForRestore();
            }
        }

    } else {
        if (!unstable) {
            if (IS_VERBOSE_THIS()){
                zInfo("⚠️ isStableOnce() returned false (structural instability)");
            }
        }
        _stableCount = 0;
    }
    // --- Snapshot frissítése a következő ciklushoz ---
    _lastGeometry = geom;
    if (!tabsGeom.isNull())
        _lastTabsGeometry = tabsGeom;
    if (!splitterSizes.isEmpty())
        _lastSplitterSizes = splitterSizes;


}

bool WindowStabilityMonitor::areWidgetsStable(const QList<QWidget*>& widgets)
{
    bool stable = true;

    for (QWidget* w : widgets) {
        WidgetSnapshot snap;
        snap.geometry = w->geometry();
        snap.size = w->size();
        snap.minSize = w->minimumSize();

        if (!_lastWidgetState.contains(w)) {
            stable = false;
        } else {
            const WidgetSnapshot& prev = _lastWidgetState[w];

            if (snap.geometry != prev.geometry ||
                snap.size != prev.size ||
                snap.minSize != prev.minSize) {

                stable = false;
            }
        }

        _lastWidgetState[w] = snap;

    }

    return stable;
}


int WindowStabilityMonitor::computeAdaptiveThreshold(int widgetCount,
                                                     bool dpiChanged,
                                                     bool profileChanged,
                                                     int recentInstability)
{
    int base = 2;

    // --- PATCH 3: új complexity thresholds ---
    int complexity = 0;
    if (widgetCount > 150) complexity = 3;
    else if (widgetCount > 80) complexity = 2;
    else if (widgetCount > 20) complexity = 1;

    // --- PATCH 3: instability penalty ---
    int instabilityPenalty = 0;
    if (recentInstability > 8) instabilityPenalty = 3;
    else if (recentInstability > 5) instabilityPenalty = 2;
    else if (recentInstability > 3) instabilityPenalty = 1;

    // --- PATCH 3: DPI és monitorprofil külön büntetés ---
    int dpiPenalty = dpiChanged ? 2 : 0;
    int profilePenalty = profileChanged ? 1 : 0;

    // --- PATCH 3: min stabilitási számlálók 2 → 1 ---
    int componentPenalty = 0;
    if (_tabsStableCount < 1)      componentPenalty++;
    if (_splitterStableCount < 1)  componentPenalty++;
    if (_childrenStableCount < 1)  componentPenalty++;

    return base
           + complexity
           + instabilityPenalty
           + dpiPenalty
           + profilePenalty
           + componentPenalty;
}


bool WindowStabilityMonitor::isFullyStable() const
{
    return !_timer.isActive();
}


