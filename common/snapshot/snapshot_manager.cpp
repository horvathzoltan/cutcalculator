#include "common/snapshot/snapshot_manager.h"

#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>
#include <QElapsedTimer>

#include "common/logger/event_logger.h"   // zEventINFO/WARN

#include "workbench_snapshot.h"

// --- Window snapshot throttle state ---
static bool g_isRestoring = false;
static QElapsedTimer g_throttleTimer;
static QString g_lastSavedGeometry;

SnapshotManager& SnapshotManager::instance()
{
    static SnapshotManager inst;
    return inst;
}

QString SnapshotManager::currentMonitorProfile(QWidget* window) const
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

/* Window snapshot */

void SnapshotManager::saveSnapshot_MainWindow(QWidget* window)
{
    if (!window) {
        zWarning("⚠️ Window snapshot save skipped: null window");
        return;
    }

    if (g_isRestoring) {
        zInfo("⏳ Window snapshot skipped: restore in progress");
        return;
    }

    if (!g_throttleTimer.isValid()) {
        g_throttleTimer.start();
    } else if (g_throttleTimer.elapsed() < 500) {
        zInfo("⏳ Window snapshot skipped: throttle (too frequent)");
        return;
    }

    if (!GeometryHelper::isWindowGeometryReady(window)) {
        zInfo("⏳ Window snapshot skipped: geometry not ready");
        return;
    }

    const QString path =
        FileNameHelper::instance().pathFor(FileKind::MainWindow_SnapshotFile,
                                           FileAccess::Write,
                                           currentMonitorProfile(window));

    const QString geom = GeometryHelper::saveWindowGeometry(window);
    if (geom.isEmpty()) {
        return;
    }

    if (geom == g_lastSavedGeometry) {
        zInfo("⏳ Window snapshot skipped: geometry unchanged");
        return;
    }
    g_lastSavedGeometry = geom;

    QString screenStr;
    if (window->screen()) {
        screenStr = GeometryHelper::serializeScreenSize(window->screen()->size());
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.setValue("Window/geometry", geom);
    snap.setValue("Window/screen",   screenStr);
    snap.sync();

    g_throttleTimer.restart();

    zInfo(QString("💾 Window snapshot saved: %1").arg(path));
}


bool SnapshotManager::restoreSnapshot_MainWindow(QWidget* window)
{
    if (!window) {
        zWarning("⚠️ Window snapshot restore skipped: null window");
        return false;
    }

    // RAII guard – minden return esetén visszaállítja a flaget
    struct RestoreGuard {
        bool* flag;
        RestoreGuard(bool* f) : flag(f) { *flag = true; }
        ~RestoreGuard() { *flag = false; }
    } guard(&g_isRestoring);


    const QString path =
        FileNameHelper::instance().pathFor(FileKind::MainWindow_SnapshotFile,
                                           FileAccess::Read,
                                           currentMonitorProfile(window));
    if (path.isEmpty()) {
        // path invalid → nincs értelme továbbmenni
        return false;
    }

    if (!QFile::exists(path)) {
        zWarning(QString("⚠️ No window snapshot file: %1").arg(path));
        return false;
    }

    QSettings snap(path, QSettings::IniFormat);
    const QString geom      = snap.value("Window/geometry").toString();
    const QString screenStr = snap.value("Window/screen").toString();
    const QSize   savedScreen = GeometryHelper::parseScreenSize(screenStr);

    if (geom.isEmpty()) {
        zWarning(QString("⚠️ Window snapshot restore: empty geometry string in file: %1").arg(path));
        return false;
    }

    // Guard-os, delayed restoreWindowGeometry – a GeometryHelper kezeli a timingot.
    GeometryHelper::restoreWindowGeometry(window, geom, savedScreen);

    g_throttleTimer.invalidate();   // restore után új throttle ciklus indul

    zInfo(QString("✅ Window snapshot restored: %1").arg(path));
    return true;
}

/* BOMWorkbench snapshot */

WorkbenchSnapshot
SnapshotManager::restoreSnapshot_BOMWorkbench(const QString& workbenchName)
{
    WorkbenchSnapshot result;

    const QString path =
        FileNameHelper::instance().pathFor(FileKind::BOM_Workbench_SnapshotFile,
                                           FileAccess::Read,
                                           workbenchName);
    if (path.isEmpty()) {
        zWarning("⚠️ Workbench snapshot: path is empty");
        return result;
    }

    if (!QFile::exists(path)) {
        zWarning(QString("ℹ️ No Workbench snapshot found at: %1").arg(path));
        return result;
    }

    QSettings snap(path, QSettings::IniFormat);
    result.leftVertical  = snap.value("Workbench/left_vertical").toString();
    result.productTypes  = snap.value("Workbench/product_types").toString();
    result.rightVertical = snap.value("Workbench/right_vertical").toString();
    result.treeHeader       = snap.value("Workbench/tree_header").toString();
    result.needRulesHeader  = snap.value("Workbench/need_rules_header").toString();
    result.modesHeader      = snap.value("Workbench/modes_header").toString();
    result.detailsHeader    = snap.value("Workbench/details_header").toString();


    zInfo(QString("ℹ️ Workbench snapshot restored: %1").arg(path));
    return result;
}

void SnapshotManager::saveSnapshot_BOMWorkbench(const WorkbenchSnapshot& s,
                                            const QString& workbenchName)
{
    const QString path =
        FileNameHelper::instance().pathFor(FileKind::BOM_Workbench_SnapshotFile,
                                           FileAccess::Write,
                                           workbenchName);

    if (path.isEmpty()) {
        zWarning("⚠️ Workbench snapshot save skipped: snapshot path is empty");
        return;
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.setValue("Workbench/left_vertical",  s.leftVertical);
    snap.setValue("Workbench/product_types",  s.productTypes);
    snap.setValue("Workbench/right_vertical", s.rightVertical);
    snap.setValue("Workbench/tree_header",    s.treeHeader);
    snap.setValue("Workbench/need_rules_header",  s.needRulesHeader);
    snap.setValue("Workbench/modes_header",       s.modesHeader);
    snap.setValue("Workbench/details_header",     s.detailsHeader);
    snap.sync();

    snap.sync();

    zInfo(QString("💾 Workbench snapshot saved: %1").arg(path));
}

QString SnapshotManager::monitorProfileFor(QWidget* w) const
{
    return currentMonitorProfile(w);
}
