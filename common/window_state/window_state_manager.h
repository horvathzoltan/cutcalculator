#pragma once

#include <QString>
#include <QSize>
#include <QWidget>

/*
 * WindowStateManager
 * -------------------
 * A top-level ablakok (MainWindow) állapotát kezeli.
 *
 * Felelősségi kör:
 *   - px-alapú geometry mentése és visszaállítása
 *   - monitorprofil- és DPI-függő állapotkezelés
 *   - restore/save időzítés (cooldown, throttle, stability)
 *
 * Kapcsolat a WidgetStateManager-rel:
 *   - a WindowState restore mindig a WidgetState restore ELŐTT fut
 *   - a WidgetState csak akkor indulhat, ha a WindowState stabil
 *
 * Kapcsolat a StateHandlers réteggel:
 *   - nincs közvetlen kapcsolat
 *   - a WidgetStateManager végzi a handler-alapú extract/restore-t
 *
 * A WindowStateManager a UI-state pipeline 1. rétege.
 */

class WindowStateManager
{
public:
    /// Globális singleton elérés – egyszerű entry point az UI-ból.
    static WindowStateManager& instance();

    /// Window geometry snapshot mentése (monitorprofilos ini-be).
    void saveSnapshot_MainWindow(QWidget* window);

    /// Window geometry snapshot visszaállítása.
    /// @return true, ha talált és sikeresen elindította a restore folyamatot.
    bool restoreSnapshot_MainWindow(QWidget* window);

    /// WidgetStateManager monitorprofilos widget-state betöltése
    QVariantMap loadWidgetState(const QString& groupName) const;

    /// WidgetStateManager monitorprofilos widget-state mentése
    void saveWidgetState(const QString& groupName, const QVariantMap& map) const;

    /// Monitor profil (pl. "1920x1080_96dpi") lekérdezése UI widgethez.
    QString monitorProfileFor(QWidget* w) const;

    void clearWidgetState(const QString& groupName) const;

private:
    WindowStateManager() = default;

    /// Belső segédfüggvény: monitorprofil generálása.
    QString currentMonitorProfile(QWidget* window) const;
};
