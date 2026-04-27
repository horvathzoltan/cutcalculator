#include "window_state_manager.h"

#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>
#include <QElapsedTimer>
#include <QDateTime>


//#include "common/logger/event_logger.h"   // zEventINFO/WARN

//#include "workbench_snapshot.h"

#include <common/utils/filename_helper.h>
#include <common/utils/window_geometry_helper.h>

// --- Window snapshot throttle state ---
static bool g_isRestoring = false;
static QElapsedTimer g_throttleTimer;
static QString g_lastSavedGeometry;
static qint64 g_restoreCooldownUntilMs = 0;

WindowStateManager& WindowStateManager::instance()
{
    static WindowStateManager inst;
    return inst;
}

QString WindowStateManager::currentMonitorProfile(QWidget* window) const
{
    // Monitorprofil: resolution + logicalDpi → "1920x1080_96dpi"
    QScreen* screen = nullptr;

    if (window && window->screen()) {
        screen = window->screen();
    } else {
        screen = QGuiApplication::primaryScreen();
    }

    if (!screen) {
        return QStringLiteral("unknown");
    }

    const QSize sz = screen->size();
    const int dpi = static_cast<int>(std::lround(screen->logicalDotsPerInch()));

    return QStringLiteral("%1x%2_%3dpi")
        .arg(sz.width())
        .arg(sz.height())
        .arg(dpi);
}

/* Window state */

void WindowStateManager::saveSnapshot_MainWindow(QWidget* window)
{
    if (!window) {
        zWarning().noquote() << "⚠️ [WindowState] Save skipped → null window";
        return;
    }

    if (g_isRestoring) {
        zInfo().noquote() << "⏳ [WindowState] Save skipped → restore in progress";
        return;
    }

    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    if (nowMs < g_restoreCooldownUntilMs) {
        zInfo().noquote() << "⏳ [WindowState] Save skipped → restore cooldown active";
        return;
    }

    if (!g_throttleTimer.isValid()) {
        g_throttleTimer.start();
    } else if (g_throttleTimer.elapsed() < 500) {
        zInfo().noquote() << "⏳ [WindowState] Save skipped → throttled (too frequent)";
        return;
    }

    if (!WindowGeometryHelper::isWindowGeometryReady(window)) {
        zInfo().noquote() << "⏳ [WindowState] Save skipped → geometry not ready";
        return;
    }

    const QString monitorProfile = currentMonitorProfile(window);
    const QString path =
        FileNameHelper::instance().pathFor(FileKind::MainWindow_StateFile,
                                           FileAccess::Write,
                                           monitorProfile);

    const QString geom = WindowGeometryHelper::saveWindowGeometry(window);
    if (geom.isEmpty()) {
        zWarning().noquote() << "⚠️ [WindowState] Save skipped → empty geometry string";
        return;
    }

    if (geom == g_lastSavedGeometry) {
        zInfo().noquote() << "⏳ [WindowState] Save skipped → geometry unchanged";
        return;
    }
    g_lastSavedGeometry = geom;

    QString screenStr;
    if (window->screen()) {
        screenStr = WindowGeometryHelper::serializeScreenSize(window->screen()->size());
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.setValue("Window/geometry", geom);
    snap.setValue("Window/screen",   screenStr);
    snap.sync();

    g_throttleTimer.restart();

    zInfo().noquote()
        << QString("💾 [WindowState] Saved → %1 (profile=%2)")
               .arg(path)
               .arg(monitorProfile);
}



bool WindowStateManager::restoreSnapshot_MainWindow(QWidget* window)
{
    if (!window) {
        zWarning().noquote() << "⚠️ [WindowState] Restore skipped → null window";
        return false;
    }

    struct RestoreGuard {
        bool* flag;
        RestoreGuard(bool* f) : flag(f) { *flag = true; }
        ~RestoreGuard() { *flag = false; }
    } guard(&g_isRestoring);

    const QString monitorProfile = currentMonitorProfile(window);
    const QString path =
        FileNameHelper::instance().pathFor(FileKind::MainWindow_StateFile,
                                           FileAccess::Read,
                                           monitorProfile);

    if (path.isEmpty()) {
        zWarning().noquote() << "⚠️ [WindowState] Restore skipped → invalid path";
        return false;
    }

    if (!QFile::exists(path)) {
        zWarning().noquote()
        << QString("⚠️ [WindowState] No state file → %1 (profile=%2)")
                .arg(path)
                .arg(monitorProfile);
        return false;
    }

    QSettings snap(path, QSettings::IniFormat);
    const QString geom      = snap.value("Window/geometry").toString();
    const QString screenStr = snap.value("Window/screen").toString();
    const QSize   savedScreen = WindowGeometryHelper::parseScreenSize(screenStr);

    if (geom.isEmpty()) {
        zWarning().noquote()
        << QString("⚠️ [WindowState] Restore skipped → empty geometry in %1")
                .arg(path);
        return false;
    }

    WindowGeometryHelper::restoreWindowGeometry(window, geom, savedScreen);

    g_restoreCooldownUntilMs = QDateTime::currentMSecsSinceEpoch() + 1000;
    g_throttleTimer.invalidate();

    zInfo().noquote()
        << QString("✅ [WindowState] Restored → %1 (profile=%2)")
               .arg(path)
               .arg(monitorProfile);

    return true;
}


QString WindowStateManager::monitorProfileFor(QWidget* w) const
{
    return currentMonitorProfile(w);
}

QVariantMap WindowStateManager::loadWidgetState(const QString& groupName) const
{
    QVariantMap result;

    const QString path =
        FileNameHelper::instance().pathFor(FileKind::UIState_File,
                                           FileAccess::Read,
                                           groupName);
    if (path.isEmpty() || !QFile::exists(path)) {
        return result;
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.beginGroup("UIState");
    const QStringList keys = snap.allKeys();
    for (const QString& k : keys) {
        result.insert(k, snap.value(k));
    }
    snap.endGroup();

    return result;
}

void WindowStateManager::clearWidgetState(const QString& groupName) const
{
    const QString path =
        FileNameHelper::instance().pathFor(FileKind::UIState_File,
                                           FileAccess::Write,
                                           groupName);
    if (path.isEmpty()) {
        return;
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.beginGroup("UIState");

    const QString prefix = groupName + "/";   // <-- FONTOS!
    const QStringList keys = snap.allKeys();

    for (const QString& k : keys) {
        if (k.startsWith(prefix))
            snap.remove(k);
    }

    snap.endGroup();
    snap.sync();
}



void WindowStateManager::saveWidgetState(const QString& groupName,
                                  const QVariantMap& map) const
{
    const QString path =
        FileNameHelper::instance().pathFor(FileKind::UIState_File,
                                           FileAccess::Write,
                                           groupName);
    if (path.isEmpty()) {
        zWarning().noquote()
        << QString("⚠️ [WidgetState::saveWidgetState] Save skipped → empty path (group=%1)")
                .arg(groupName);
        return;
    }

    if (map.isEmpty()) {
        zWarning().noquote()
        << QString("⚠️ [WidgetState::saveWidgetState] Empty map → no keys written (group=%1, path=%2)")
                .arg(groupName, path);
        return;
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.beginGroup("UIState");
    for (auto it = map.begin(); it != map.end(); ++it) {
        snap.setValue(it.key(), it.value());
    }
    snap.endGroup();
    snap.sync();

    zInfo().noquote()
        << QString("💾 [WidgetState::saveWidgetState] QSettings sync completed → %1 (keys=%2)")
               .arg(path)
               .arg(map.size());
}
