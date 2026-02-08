#include "common/snapshot/snapshot_manager.h"

#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>

#include "common/logger/event_logger.h"   // zEventINFO/WARN

#include "workbench_snapshot.h"

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

QString SnapshotManager::snapshotFilePathFor(QWidget* window) const
{
    if (!window) {
        return {};
    }

    const QString profile = currentMonitorProfile(window);
    const QString path =
        FileNameHelper::instance().pathFor(FileKind::UiSnapshotFile, FileAccess::Read, profile);

    if (path.isEmpty()) {
        zWarning("⚠️ SnapshotManager: snapshot path is empty");
        return {};
    }

    return path;
}

/* Window snapshot */

void SnapshotManager::saveWindowSnapshot(QWidget* window)
{
    if (!window) {
        zWarning("⚠️ Window snapshot save skipped: null window");
        return;
    }

    const QString path = snapshotFilePathFor(window);
    if (path.isEmpty()) {
        // Log már megtörtént snapshotFilePathFor-ben
        return;
    }

    // Percent-based geometry mentés
    const QString geom = GeometryHelper::saveWindowGeometry(window);
    if (geom.isEmpty()) {
        // GeometryHelper már logolta, miért üres
        return;
    }

    // Screen méret audit célra
    QString screenStr;
    if (window->screen()) {
        screenStr = GeometryHelper::serializeScreenSize(window->screen()->size());
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.setValue("Window/geometry", geom);
    snap.setValue("Window/screen",   screenStr);
    snap.sync();

    const QString profile = currentMonitorProfile(window);
    zInfo(QString("💾 Geometry snapshot saved for profile '%1' → %2")
                   .arg(profile, path));
}

bool SnapshotManager::restoreWindowSnapshot(QWidget* window)
{
    if (!window) {
        zWarning("⚠️ Window snapshot restore skipped: null window");
        return false;
    }

    const QString path = snapshotFilePathFor(window);
    if (path.isEmpty()) {
        // path invalid → nincs értelme továbbmenni
        return false;
    }

    if (!QFile::exists(path)) {
        const QString profile = currentMonitorProfile(window);
        zWarning(QString("⚠️ No geometry snapshot for profile '%1'").arg(profile));
        return false;
    }

    QSettings snap(path, QSettings::IniFormat);
    const QString geom      = snap.value("Window/geometry").toString();
    const QString screenStr = snap.value("Window/screen").toString();
    const QSize   savedScreen = GeometryHelper::parseScreenSize(screenStr);

    if (geom.isEmpty()) {
        zWarning(QString("⚠️ Snapshot restore: empty geometry string in '%1'").arg(path));
        return false;
    }

    // Guard-os, delayed restoreWindowGeometry – a GeometryHelper kezeli a timingot.
    GeometryHelper::restoreWindowGeometry(window, geom, savedScreen);

    const QString profile = currentMonitorProfile(window);
    zInfo(QString("✅ Window restored from geometry snapshot '%1' (%2)")
                   .arg(profile, path));
    return true;
}

/* BOMWorkbench snapshot */

WorkbenchSnapshot
SnapshotManager::loadWorkbenchSnapshot(QWidget* contextWindow)
{
    WorkbenchSnapshot result;

    if (!contextWindow) {
        zWarning("⚠️ loadWorkbenchSnapshot skipped: null contextWindow");
        return result;
    }

    const QString path = snapshotFilePathFor(contextWindow);
    if (path.isEmpty()) {
        zWarning("⚠️ loadWorkbenchSnapshot: snapshot path is empty");
        return result;
    }

    if (!QFile::exists(path)) {
        const QString profile = currentMonitorProfile(contextWindow);
        zWarning(QString("ℹ️ No BOMWorkbench snapshot for profile '%1'").arg(profile));
        return result;
    }

    QSettings snap(path, QSettings::IniFormat);
    result.leftVertical  = snap.value("Workbench/left_vertical").toString();
    result.productTypes  = snap.value("Workbench/product_types").toString();
    result.rightVertical = snap.value("Workbench/right_vertical").toString();
    result.treeHeader    = snap.value("Workbench/tree_header").toString();

    return result;
}

void SnapshotManager::saveWorkbenchSnapshot(const WorkbenchSnapshot& s,
                                            QWidget* contextWindow)
{
    if (!contextWindow) {
        zWarning("⚠️ BOMWorkbench snapshot skipped: null contextWindow");
        return;
    }

    if (!GeometryHelper::isWindowGeometryReady(contextWindow)) {
        // * ha a window még nem stabil, akkor snapshot mentés csak zaj lenne.
        zWarning("⏳ Workbench snapshot skipped: window not ready");
        return;
    }

    const QString path = snapshotFilePathFor(contextWindow);
    if (path.isEmpty()) {
        zWarning("⚠️ BOMWorkbench snapshot skipped: snapshot path is empty");
        return;
    }

    QSettings snap(path, QSettings::IniFormat);
    snap.setValue("Workbench/left_vertical",  s.leftVertical);
    snap.setValue("Workbench/product_types",  s.productTypes);
    snap.setValue("Workbench/right_vertical", s.rightVertical);
    snap.setValue("Workbench/tree_header",    s.treeHeader);
    snap.sync();

    const QString profile = currentMonitorProfile(contextWindow);
    zInfo(QString("💾 BOMWorkbench snapshot saved for profile '%1' → %2")
                   .arg(profile, path));
}

QString SnapshotManager::monitorProfileFor(QWidget* w) const
{
    return currentMonitorProfile(w);
}
