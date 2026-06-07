#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>
#include <QStackedWidget>

#include <common/ui_state/i_workbench_custom_state.h>

#include <common/ui/crud/actionmap.h>

#include "common/ui/crud/list_toolbar_factory.h"
#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"
#include "orders/view/order_header_list_panel.h"
#include "orders/presenter/order_workbench_presenter.h"
#include "orders/manager/order_manager.h"

class OrderWorkbench : public QWidget, public IWorkbenchCustomState  {
    Q_OBJECT
public:
    explicit OrderWorkbench(QWidget* parent = nullptr);

    enum class State {

        //
        // 0) INITIAL EMPTY
        // ------------------------------------------------------------------
        // A rendszer teljesen üres:
        //   • OrderHeaderRegistry üres (nincs egyetlen mentett rendelés sem)
        //   • és a panelen sincs még header (originalOpt = nullopt)
        //
        // UI:
        //   • bal oldali lista helyett placeholder
        //   • header panel helyett placeholder
        //   • item table helyett placeholder
        //   • csak a “+ Új” gomb aktív
        //
        InitialEmpty,


        //
        // 1) NEW HEADER EDITING
        // ------------------------------------------------------------------
        // Új rendelést hozunk létre, de MÉG NEM módosítottuk.
        // Feltételek:
        //   • registry üres VAGY a panelen lévő header ID nincs a registryben
        //   • M = false (nincs módosítás)
        //
        // UI:
        //   • header panel látszik
        //   • lista és item table NEM látszik
        //   • Save/Cancel NEM aktív
        //
        NewHeaderEditing,


        //
        // 2) NEW HEADER EDITED
        // ------------------------------------------------------------------
        // Új rendelést hozunk létre, és MÁR módosítottuk.
        // Feltételek:
        //   • registry üres VAGY a panelen lévő header ID nincs a registryben
        //   • M = true (van módosítás)
        //
        // UI:
        //   • header panel látszik
        //   • lista és item table NEM látszik
        //   • Save/Cancel AKTÍV
        //
        NewHeaderEdited,


        //
        // 3) LIST NO SELECTION
        // ------------------------------------------------------------------
        // A registry NEM üres, de a felhasználó nem választott ki semmit.
        // Feltételek:
        //   • registry nem üres
        //   • originalOpt = nullopt (panel üres)
        //
        // UI:
        //   • bal oldali lista látszik
        //   • header panel helyett placeholder
        //   • item table helyett placeholder
        //
        ListNoSelection,


        //
        // 4) HEADER SELECTED
        // ------------------------------------------------------------------
        // Egy meglévő rendelés van kiválasztva, és NINCS módosítva.
        // Feltételek:
        //   • registry nem üres
        //   • originalOpt létezik
        //   • original.id megtalálható a registryben (E = true)
        //   • M = false
        //
        // UI:
        //   • lista látszik
        //   • header panel látszik
        //   • item table látszik
        //   • Modify/Delete aktív
        //   • Save/Cancel NEM aktív
        //
        HeaderSelected,


        //
        // 5) HEADER EDITING
        // ------------------------------------------------------------------
        // Egy meglévő rendelést szerkesztünk, és MÓDOSULT.
        // Feltételek:
        //   • registry nem üres
        //   • originalOpt létezik
        //   • original.id megtalálható a registryben (E = true)
        //   • M = true
        //
        // UI:
        //   • lista látszik
        //   • header panel látszik
        //   • item table látszik
        //   • Save/Cancel aktív
        //
        HeaderEditing
    };


    struct UiState {
        bool listVisible;
        bool listPlaceholderVisible;

        bool headerVisible;
        bool headerPlaceholderVisible;

        bool itemTableVisible;
        bool itemPlaceholderVisible;
        bool itemToolbarVisible;

        bool headerAddEnabled;
        bool headerModifyEnabled;
        bool headerDeleteEnabled;
        bool headerSaveEnabled;
        bool headerCancelEnabled;

        bool itemAddEnabled;
        bool itemDeleteEnabled;
    };

    //
    // ──────────────────────────────────────────────────────────────────────────────
    //   OrderWorkbench::UIModel – A Workbench logikai UI-modellje
    // ──────────────────────────────────────────────────────────────────────────────
    //
    // A UIModel NEM egy widget-dump és NEM a Builder belső szerkezetének tükre.
    // A UIModel kizárólag azokat az UI-elemeket tartalmazza, amelyekre a Workbench
    // vagy a Presenter LOGIKAILAG hivatkozik.
    //
    // A modell célja:
    //   • stabil, hosszú távon változatlan API biztosítása a Workbench számára
    //   • a Builder és a Workbench közötti coupling minimalizálása
    //   • a Workbench állapotgépének és viselkedésének támogatása
    //   • a Presenter számára szükséges UI-elemek átadása
    //
    // ──────────────────────────────────────────────────────────────────────────────
    //   Mi kerülhet a UIModel-be?
    // ──────────────────────────────────────────────────────────────────────────────
    //
    // 1) Logikai UI-elemek:
    //    • panelek (listPanel, headerPanel, itemTable)
    //    • placeholder widgetek (listPlaceholder, headerPlaceholder, itemPlaceholder)
    //    • overlay widgetek (headerOverlay, itemOverlay)
    //    • ActionMap-ek (listActions)
    //    • toolbarok, amelyeket a Workbench állapotgép kapcsol (itemToolbar)
    //
    // 2) Olyan elemek, amelyeket a Workbench vagy a Presenter
    //    KÖZVETLENÜL használ:
    //    • láthatóság kapcsolása
    //    • engedélyezés/tiltás
    //    • callback binding
    //    • overlay frissítés
    //
    // ──────────────────────────────────────────────────────────────────────────────
    //   Mi NEM kerülhet a UIModel-be?
    // ──────────────────────────────────────────────────────────────────────────────
    //
    // 1) Pusztán vizuális elemek:
    //    • listToolbar
    //    • headerToolbar
    //    • layoutok
    //    • splitterek
    //    • label-ek
    //
    // 2) Olyan widgetek, amelyeket a Workbench NEM használ logikailag:
    //    • toolbarok, amelyeket nem kapcsol a state machine
    //    • UI-elemek, amelyek csak a Builder layoutjához kellenek
    //
    // 3) A Builder belső szerkezete:
    //    • a UIModel nem a Builder tükre
    //    • a Builder szabadon változhat anélkül, hogy a Workbench törne
    //
    // ──────────────────────────────────────────────────────────────────────────────
    //   Filozófia
    // ──────────────────────────────────────────────────────────────────────────────
    //
    // A UIModel a Workbench LOGIKAI nézete a UI-ról.
    // Nem célja minden widget tárolása, csak azoké, amelyek a Workbench működéséhez
    // szükségesek.
    //
    // A Builder feladata:
    //   • UI-elemek létrehozása
    //   • layout összeállítása
    //   • toolbarok elhelyezése
    //
    // A Workbench feladata:
    //   • logikai viselkedés (state machine)
    //   • callback binding
    //   • overlay frissítés
    //   • placeholder logika
    //
    // A Presenter feladata:
    //   • domain logika
    //   • adatok mozgatása a view és a model között
    //
    // A UIModel ennek a három szereplőnek a tiszta határfelülete.
    //
    // ──────────────────────────────────────────────────────────────────────────────
    //

    struct UIModel {
        QWidget* listContainer;
        QWidget* headerContainer;
        QWidget* itemContainer;

        OrderHeaderListPanel* listPanel;
        OrderHeaderPanel* headerPanel;
        OrderItemTable* itemTable;

        QWidget* listPlaceholder;
        QWidget* headerPlaceholder;
        QWidget* itemPlaceholder;

        QToolBar* itemToolbar;
        RepositoryOverlayWidget<OrderHeaderRegistry>* headerOverlay;
        RepositoryOverlayWidget<OrderItemRegistry>* itemOverlay;

        ActionMap<ListAction> listActions;
    };

private:
    OrderWorkbenchPresenter* _presenter = nullptr;
    OrderManager* _orderManager = nullptr;

    UIModel _ui;

    void showHeaderPlaceholder(bool show);
    void showItemPlaceholder(bool show);

    // placeholder szöveg frissítése állapot alapján
    void updateHeaderPlaceholderText(State s);
    void updateItemPlaceholderText(State s);

    bool isHeaderModified() const;
public:
    QVariantMap saveCustomState() const override;
    void restoreCustomState(const QVariantMap& state) override;

    State resolveState() const;
    void applyState(State s);

    QString stateToString(State s) const;

    Q_INVOKABLE void postRestoreFix();
    void updateUIState();
};
