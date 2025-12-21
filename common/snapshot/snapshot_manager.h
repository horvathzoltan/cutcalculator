#pragma once

#include <QString>
#include <QSize>

#include "common/settings/settings_manager.h"      // WorkbenchSnapshot típus
#include "common/utils/geometry_helper.h"
#include "common/utils/filename_helper.h"
#include "workbench_snapshot.h"

/**
 * @class SnapshotManager
 * @brief Monitorprofilhoz kötött UI snapshot kezelés (window + BOMWorkbench).
 *
 * Felelősség:
 * - Window geometry snapshot mentése/visszaállítása (per monitor profil).
 * - BOMWorkbench splitter/header snapshot mentése/visszaállítása.
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
    void saveWindowSnapshot(QWidget* window);

    /// Window geometry snapshot visszaállítása.
    /// @return true, ha talált és sikeresen elindította a restore folyamatot.
    bool restoreWindowSnapshot(QWidget* window);

    /// BOMWorkbench snapshot betöltése (splitters + header percent stringek).
    WorkbenchSnapshot loadWorkbenchSnapshot(QWidget* contextWindow);

    /// BOMWorkbench snapshot mentése.
    void saveWorkbenchSnapshot(const WorkbenchSnapshot& s,
                               QWidget* contextWindow);

    /// Monitor profil (pl. "1920x1080_96dpi") lekérdezése UI widgethez.
    QString monitorProfileFor(QWidget* w) const;

private:
    SnapshotManager() = default;

    /// Belső segédfüggvény: monitorprofil generálása.
    QString currentMonitorProfile(QWidget* window) const;

    /// Snapshot ini file path feloldása adott widgethez (monitorprofil alapján).
    QString snapshotFilePathFor(QWidget* window) const;
};
