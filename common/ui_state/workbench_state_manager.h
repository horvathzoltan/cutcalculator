#pragma once
#include <QObject>
#include <QMap>
#include <QString>

//
// ──────────────────────────────────────────────────────────────────────────────
//   WORKBENCH STATE MANAGER – PURPOSE AND SCOPE
// ──────────────────────────────────────────────────────────────────────────────
//
// A WorkbenchStateManager kizárólag a *UI snapshot* kezelésére szolgál.
// Ez azt jelenti, hogy automatikusan menti és visszaállítja a workbench
// vizuális állapotát, amikor a user tabot vált vagy bezárja az alkalmazást.
//
// FONTOS: ez NEM a workbench belső logikai állapotának mentésére készült.
//
// Mit ment?
//   ✔ QSplitter méretek (arányok)
//   ✔ QHeaderView oszlopszélességek, sorrend, rendezési irány
//   ✔ QAbstractScrollArea scroll pozíciók
//   ✔ QTabWidget aktuális tab index
//   ✔ minden más widget geometria, amit a WidgetStateManager támogat
//
// Mit NEM ment?
//   ✘ kiválasztott elem (pl. melyik order ID volt kijelölve)
//   ✘ szűrőmezők tartalma
//   ✘ keresőmezők tartalma
//   ✘ domain logikai állapot
//   ✘ Presenter állapot
//   ✘ bármilyen olyan adat, ami eseményeket váltana ki restore közben
//
// Miért nem ment belső állapotot?
//   - A snapshot visszaállítás UI eseményeket generál (selectRow, setText, stb.)
//   - Ezek Presenter hívásokat indítanak (loadOrder, rebuildList, stb.)
//   - Ez "robotizálná" a workbenchet: nem a user, hanem a restore futtatná a logikát
//   - Ez instabilitáshoz, hibákhoz és nem determinisztikus viselkedéshez vezetne
//
// Hogyan működik?
//   - A MainWindow tabváltáskor meghívja: onTabActivated(widget)
//   - Ez egyszeri restore-t végez a WidgetStateManager segítségével
//   - Tab elhagyásakor: onTabDeactivated(widget)
//   - Ez snapshot mentést végez (szinkron vagy aszinkron)
//   - Kilépéskor: beginClosing() → minden snapshot szinkron mentése
//
// Mikor használjuk?
//   - Ha a workbench UI-jának vizuális állapotát akarjuk megőrizni
//   - Ha a user visszatér egy tabra, és ugyanazt a vizuális elrendezést várja
//
// Mikor NEM használjuk?
//   - Ha belső logikai állapotot akarunk menteni (pl. kiválasztott order ID)
//   - Ha domain adatot akarunk menteni
//   - Ha olyan állapotot akarunk menteni, ami eseményeket váltana ki restore közben
//
// Ha belső állapotot kell menteni:
//   → azt a workbench saját maga tegye meg (ID-alapon), külön mechanizmussal,
//     és restore közben QSignalBlocker-rel blokkolja az eseményeket.
//
// ──────────────────────────────────────────────────────────────────────────────
//


class QWidget;

class WorkbenchStateManager : public QObject {
    Q_OBJECT
public:
    static WorkbenchStateManager& instance();

    void registerWorkbench(const QString& id, QWidget* wb);
    void onTabActivated(QWidget* wb);
    void onTabDeactivated(QWidget* wb, bool immediate = false);

    void beginClosing();

    bool isShown(QWidget *wb);
private:
    bool _closing = false;

    struct WBInfo {
        QWidget* widget = nullptr;
        bool wasShown = false;
    };

    QMap<QString, WBInfo> _workbenches;

    explicit WorkbenchStateManager(QObject* parent = nullptr);
    QString findIdByWidget(QWidget* wb) const;
};
