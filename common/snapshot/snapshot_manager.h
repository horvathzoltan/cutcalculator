#pragma once

#include <QString>
#include <QSize>
#include <QWidget>

// #include "common/utils/geometry_helper.h"
// #include "common/utils/filename_helper.h"

/**
 * @class SnapshotManager
 * @brief UI snapshot kezelés (Window monitorprofilos + Workbench monitorfüggetlen).
 *
 * Felelősség:
 * - Window geometry snapshot mentése/visszaállítása (per monitor profil).
 * - Workbench splitter/header snapshot mentése/visszaállítása (monitorfüggetlen).
 * - Snapshot file path feloldása FileNameHelper alapján (ui_snapshots alatt).
 * - Guardolt, auditbarát logika (zEventINFO/WARN).
 *
 * Design:
 * - Stateless, de singleton entry point a kényelmes eléréshez.
 * - A tényleges számításokat a GeometryHelper végzi (percent-based).
 * - Nem keveredik a settings.ini fallback logikával (az UiDefaultStore felelőssége).
 */
class SnapshotManager
{
public:
    /// Globális singleton elérés – egyszerű entry point az UI-ból.
    static SnapshotManager& instance();

    /// Window geometry snapshot mentése (monitorprofilos ini-be).
    void saveSnapshot_MainWindow(QWidget* window);

    /// Window geometry snapshot visszaállítása.
    /// @return true, ha talált és sikeresen elindította a restore folyamatot.
    bool restoreSnapshot_MainWindow(QWidget* window);

    /// BOMWorkbench snapshot betöltése (splitters + header percent stringek).
    /// BOMWorkbench snapshot mentése.

    //WorkbenchSnapshot restoreSnapshot_BOMWorkbench(const QString& workbenchName);

    // void saveSnapshot_BOMWorkbench(const WorkbenchSnapshot& s,
    //                                const QString& workbenchName);

    /// UIStateCollector monitorprofilos UI-state betöltése
    QVariantMap loadUIState(const QString& groupName) const;

    /// UIStateCollector monitorprofilos UI-state mentése
    void saveUIState(const QString& groupName, const QVariantMap& map) const;

    /// Monitor profil (pl. "1920x1080_96dpi") lekérdezése UI widgethez.
    QString monitorProfileFor(QWidget* w) const;

    void clearUIState(const QString& groupName) const;

private:
    SnapshotManager() = default;

    /// Belső segédfüggvény: monitorprofil generálása.
    QString currentMonitorProfile(QWidget* window) const;
};
