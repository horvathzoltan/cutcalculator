// common/startup/startup_status_manager.h
#pragma once

#include "common/startup/startup_status.h"

/**
 * @brief Központi manager az indítási folyamat státuszának kezelésére.
 *
 * Feladatok:
 * - StartupStatus értelmezése és felhasználóbarát üzenetek megjelenítése (QMessageBox).
 * - Audit-barát logolás a Logger rendszeren keresztül (zError/zWarning).
 * - Rövid "preview" a figyelmeztetésekből (konfigurálható darabszám).
 *
 * Használat:
 * @code
 * StartupManager manager;
 * StartupStatus status = manager.runStartupSequence();
 * if (!StartupStatusManager::handle(status)) {
 *     return -1; // kritikus hiba → leállás
 * }
 * @endcode
 */
class StartupStatusManager {
public:
    /**
     * @brief Teljes startup státusz kezelése.
     * @param status A StartupManager által visszaadott státusz.
     * @return true, ha folytatható az indítás; false, ha kritikus hiba történt.
     *
     * Viselkedés:
     * - isSuccess() == false → zError + critical MessageBox + false.
     * - warnings().isEmpty() == false → zWarning soronként + barátságos Warning MessageBox.
     * - Egyébként nem történik GUI jelzés.
     */
    static bool handle(const StartupStatus& status);

    /**
     * @brief Beállítja, hány figyelmeztetést jelenítsen meg előnézetként a dialógusban.
     * @param count 0 → nincs előnézet; >0 → ennyi sor jelenik meg.
     */
    static void setPreviewLimit(int count);

    /**
     * @brief Beállítja, hogy a warning dialógus végén jelenjen-e meg a "Részletek a logban" jelzés.
     * @param enabled true → felirat megjelenik; false → elhagyjuk.
     */
    static void setShowLogHint(bool enabled);

    /**
     * @brief Opcionálisan megadható egy emberbarát logfájl-útvonal a dialógushoz (pl. errors_YYYYMMDD.txt).
     * @param path Rövid elérési út vagy fájlnév; üresen hagyható.
     */
    static void setHumanLogPath(QString path);

private:
    static int s_previewLimit;
    static bool s_showLogHint;
    static QString s_humanLogPath;

    // Segédfüggvények a rövid, olvasható üzenetek összeállításához.
    static QString makeErrorSummary(const QString& error);
    static QString makeWarningSummary(const QStringList& warnings);
};
