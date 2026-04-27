#pragma once

#include <QWidget>
#include <QHash>
#include <QSettings>
#include <functional>

#include <QSplitter>
#include <QHeaderView>
#include <QTabWidget>
#include <QAbstractScrollArea>

/*
 * WidgetStateManager
 * -------------------
 * A belső widgetek állapotát kezeli (splitter, tabwidget, headerview, scrollarea).
 *
 * Felelősségi kör:
 *   - widgetfa bejárása (WidgetDiscoveryHelper)
 *   - QVariantMap buffer kezelése (WidgetStateSettings)
 *   - extract/restore meghívása (StateHandlers)
 *
 * Kapcsolat a WindowStateManager-rel:
 *   - csak akkor fut restore, ha a WindowState már stabil
 *   - a WindowStateManager végzi a fájlkezelést (load/save/clear)
 *
 * Kapcsolat a StateHandlers réteggel:
 *   - minden widgethez handler-t keres (findHandler)
 *   - a handler végzi a tényleges extract/restore műveletet
 *
 * A WidgetStateManager a UI-state pipeline 2. rétege.
 */

/*
 * WidgetStateManager – Integrációs áttekintés (PATCH 11)
 * -------------------------------------------------------
 * A WidgetStateManager a belső widgetek állapotának mentéséért és visszaállításáért
 * felelős. A rendszer négy jól elkülönülő komponensből áll:
 *
 *   1) WidgetDiscoveryHelper
 *      - Rekurzívan bejárja a widgetfát
 *      - Kiszűri a nem releváns widgeteket (dekorációk, egyszerű kontrollok, Qt belső elemek)
 *      - Csak layout‑kritikus widgeteket ad vissza (splitter, tabwidget, scrollarea, stb.)
 *
 *   2) WidgetStateSettings
 *      - Betölti a groupName‑hez tartozó QVariantMap‑et a WindowStateManager‑ből
 *      - A handler‑ek ebbe írják az extract során a kulcsokat
 *      - save() visszamenti a módosított map‑et
 *      - clearUIState() teljes UIState törlést végez (PATCH 10)
 *
 *   3) StateHandlers (extract/restore)
 *      - Widget‑specifikus állapotkezelés
 *      - extract: widget → QVariantMap (percent‑alapú, kulcs‑prefixelt)
 *      - restore: QVariantMap → widget
 *      - Egységes logolás: [WidgetState] prefix, audit‑barát formátum
 *
 *   4) WidgetStateManager (ez a komponens)
 *      - Orchestrator szerep: összehangolja a fenti három réteget
 *      - saveWidgetState():
 *          a) WidgetDiscoveryHelper::collect()
 *          b) WidgetStateSettings buffer létrehozása
 *          c) audit‑adatok beírása
 *          d) handler‑alapú extract
 *          e) save()
 *
 *      - restoreWidgetState():
 *          a) geometry readiness ellenőrzése (WindowState restore után)
 *          b) WidgetDiscoveryHelper::collect()
 *          c) handler‑alapú restore
 *
 * Fontos architekturális elvek:
 *   - A WidgetState percent‑alapú → monitorfüggetlen
 *   - A WindowState px‑alapú → monitorprofil‑függő
 *   - A WidgetState restore mindig a WindowState restore UTÁN fut
 *   - A WidgetState NEM ment top‑level geometry‑t, csak belső widgeteket
 *
 * A PATCH 11 célja:
 *   - kommentek és logok modernizálása
 *   - architektúra egyértelmű dokumentálása
 *   - audit‑barát, két‑rétegű UI‑state modell rögzítése
 *
 * A működés NEM változott — kizárólag a dokumentáció és logolás lett egységesítve.
 */


class WidgetStateManager
{
public:
    explicit WidgetStateManager(const QString& groupName);

    void saveWidgetState(QWidget* root);
    void restoreWidgetState(QWidget* root);

private:
    QString _groupName;

    using ExtractFn = std::function<void(QWidget*, QVariantMap&, const QString&)>;
    using RestoreFn = std::function<void(QWidget*, QVariantMap&, const QString&)>;

    struct Handler {
        ExtractFn extract;
        RestoreFn restore;
    };

    QHash<QByteArray, Handler> _handlers;

    QString computeWidgetKey(QWidget* w) const;

    template<typename T>
    void registerHandler(ExtractFn e, RestoreFn r)
    {
        _handlers.insert(T::staticMetaObject.className(), Handler{e, r});
    }

    const Handler* findHandler(const QMetaObject* mo) const;
    static QString computeWidgetPath(QWidget *w);

    QString filePath() const;
};
