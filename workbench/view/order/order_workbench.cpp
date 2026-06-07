#include "order_workbench.h"
#include "order_workbench_ui_builder.h"
#include <common/ui_state/workbench_state_manager.h>
#include <QTimer>
#include <optional>
#include <orders/repository/order_item_repository.h>
#include <common/ui/crud/list_toolbar_factory.h>

OrderWorkbench::OrderWorkbench(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("order_workbench");

    // ⭐ REGISTER WORKBENCH
    WorkbenchStateManager::instance().registerWorkbench("order_workbench", this);

    // UI Builder
    _ui = OrderWorkbenchUIBuilder::build(this);

    // Presenter
    _orderManager = new OrderManager(this);
    _presenter = new OrderWorkbenchPresenter(
        _ui.headerPanel,
        _ui.itemTable,
        _orderManager,
        _ui.listPanel,
        _ui.headerOverlay,
        _ui.itemOverlay,
        this
        );

    // LIST TOOLBAR CALLBACKEK BEKÖTÉSE
    _ui.listActions.connect(ListAction::Add,
                           _presenter,
                           &OrderWorkbenchPresenter::newOrder);

    _ui.listActions.connect(ListAction::Delete,
                           _presenter,
                           &OrderWorkbenchPresenter::deleteCurrentOrder);

    _ui.listActions.connect(ListAction::Rename,
                           _presenter,
                           &OrderWorkbenchPresenter::renameCurrentOrder);

    _ui.listActions.connect(ListAction::Refresh,
                           _ui.listPanel,
                           &OrderHeaderListPanel::refresh);

    // Lista kiválasztás → Presenter
    connect(_ui.listPanel, &OrderHeaderListPanel::orderSelected,
            this, [this](const QUuid& id) {
                _presenter->loadOrder(id);
            });


    // 6) UI state
    updateUIState();

    _presenter->refreshHeaderOverlay();
}

QVariantMap OrderWorkbench::saveCustomState() const
{
    QVariantMap m;

    if (_ui.listPanel) {
        auto id = _ui.listPanel->selectedOrderId();
        if (!id.has_value())
            m.insert("selectedOrderId", id.value());
    }

    return m;
}

void OrderWorkbench::restoreCustomState(const QVariantMap& state)
{
    if (!_ui.listPanel)
        return;

    QSignalBlocker b1(_ui.listPanel);

    const QString idStr = state.value("selectedOrderId").toString();
    if (idStr.isEmpty())
        return;

    QUuid id(idStr);
    if (id.isNull())
        return;

    _ui.listPanel->silentSelectById(id);
}

void OrderWorkbench::showHeaderPlaceholder(bool show)
{
    if (!_ui.headerPlaceholder || !_ui.headerPanel)
        return;

    _ui.headerPlaceholder->setVisible(show);
    _ui.headerPanel->setVisible(!show);
}


void OrderWorkbench::showItemPlaceholder(bool show)
{
    if (!_ui.itemPlaceholder || !_ui.itemTable)
        return;

    _ui.itemPlaceholder->setVisible(show);
    _ui.itemTable->setVisible(!show);

    if (_ui.itemToolbar)
        _ui.itemToolbar->setVisible(!show);
}


Q_INVOKABLE void OrderWorkbench::postRestoreFix()
{
    bool leftEmpty  = (OrderHeaderRegistry::instance().size() == 0);
    bool rightEmpty = true;

    if (!(leftEmpty && rightEmpty))
        return;

    // 1) FŐ SPLITTER fix – ez mehet 0 ms késleltetéssel
    QTimer::singleShot(0, this, [this]() {

        zInfo("postRestoreFix → fő splitter fix");
        auto* _splitter = this->findChild<QSplitter*>("order_main_splitter");
        if(_splitter)
            _splitter->setSizes({300, 700});

        // 2) JOBB OLDALI SPLITTER fix – csak akkor, amikor már van height
        QTimer::singleShot(0, this, [this]() {

            auto* _rightSplitter = this->findChild<QSplitter*>("order_right_splitter");
            if(_rightSplitter) {
                int h = _rightSplitter->height();
                if (h <= 0)
                    return;

                int top = h * 0.3;
                int bottom = h - top;
                _rightSplitter->setSizes({top, bottom});

                zInfo("postRestoreFix → jobb splitter fix (2‑container OK)");
            }
        });

    });
}

// UiState mátrix – minden Workbench állapot deklaratív UI-leképezése

static const QMap<OrderWorkbench::State, OrderWorkbench::UiState> UI_STATE_MAP = {

// 1) Teljesen üres rendszer – nincs header, nincs kiválasztás
{
    OrderWorkbench::State::InitialEmpty,
    {
        false, true,   // listVisible, listPlaceholderVisible
        false, true,   // headerVisible, headerPlaceholderVisible
        false, true, false,   // itemTableVisible, itemPlaceholderVisible, itemToolbarVisible
        true, false, false, false, false,   // headerAdd,Modify,Delete,Save,Cancel
        false, false   // itemAdd, itemDelete
    }
},

    // 2) Új header létrehozása – még nincs mentve
    {
        OrderWorkbench::State::NewHeaderEditing,
        {
            false, true,
            true,  false,
            false, true, false,
            false, false, false, false, false,
            false, false
        }
    },

    {
        OrderWorkbench::State::NewHeaderEdited,
        {
            false, true,
            true,  false,
            false, true, false,
            false, false, false, true, true,
            false, false
        }
    },

    // 3) Van mentett header, de nincs kiválasztva
    {
        OrderWorkbench::State::ListNoSelection,
        {
            true,  false,
            false, true,
            false, true, false,
            true, false, false, false, false,
            false, false
        }
    },

    // 4) Van kiválasztott header
    {
        OrderWorkbench::State::HeaderSelected,
        {
            true,  false,
            true,  false,
            true,  false, true,
            true, true, true, false, false,
            true, true
        }
    },

// 5) Meglévő header szerkesztése
{
    OrderWorkbench::State::HeaderEditing,
    {
        true,  false,
            true,  false,
            true,  false, true,
            false, false, false, true, true,
            true, true
    }
}
};

void OrderWorkbench::updateUIState()
{
    auto state = resolveState();
    zInfo("WorkbenchState: "+ stateToString(state));
    applyState(state);
}

// OrderWorkbench::State OrderWorkbench::resolveState() const
// {
//     // 1) Lista kiválasztás (optional)
//     std::optional<QUuid> selectedId;
//     if (_ui.listPanel)
//         selectedId = _ui.listPanel->selectedOrderId();   // optional

//     // 2) HeaderPanel ID (optional)
//     std::optional<QUuid> panelId = _ui.headerPanel->headerId();

//     // 3) Létezik-e a panelen lévő header a registryben?
//     bool panelIdExists = false;
//     if (panelId.has_value())
//         panelIdExists = OrderHeaderRegistry::instance().existsById(*panelId);

//     // --- ÁLLAPOTGÉP ---

//     // 0) Induló állapot: nincs panelId és nincs selectedId
//     if (!panelId.has_value() && !selectedId.has_value())
//         return State::InitialEmpty;

//     // 1) Új header létrehozása (panelId nincs a registryben)
//     if (panelId.has_value() && !panelIdExists)
//         return State::NewHeaderEditing;

//     // 2) Van header a panelen, de nincs kiválasztva semmi a listában
//     if (!selectedId.has_value())
//         return State::ListNoSelection;

//     // 3) A panelen lévő header van kiválasztva
//     if (panelId.has_value() && selectedId.value() == panelId.value()) {
//         if (isHeaderModified())
//             return State::HeaderEditing;
//         return State::HeaderSelected;
//     }

//     // 4) Más header van kiválasztva
//     return State::HeaderSelected;
// }


OrderWorkbench::State OrderWorkbench::resolveState() const
{
    auto originalOpt = _ui.headerPanel->originalHeader();
    bool M = isHeaderModified();

    // 0) Ha a registry üres → InitialEmpty
    if (OrderHeaderRegistry::instance().isEmpty()){
        if(!originalOpt)
            return State::InitialEmpty;
        else{
            if(M)
                return State::NewHeaderEdited;
            else
                return State::NewHeaderEditing;
        }
    }

    // 1) Ha nincs original → InitialEmpty
    if (!originalOpt)
        return State::InitialEmpty;

    const OrderHeader& original = *originalOpt;

    // 2) Meglévő header?
    bool E = OrderHeaderRegistry::instance().existsById(original.id);

    // 3) Új header
    if (!E) {
        if (M)
            return State::HeaderEditing;
        return State::NewHeaderEditing;
    }

    // 4) Meglévő header
    if (M)
        return State::HeaderEditing;

    return State::HeaderSelected;
}



void OrderWorkbench::applyState(State s)
{
    const UiState u = UI_STATE_MAP.value(s);

    _ui.listPanel->setVisible(u.listVisible);
    _ui.listPlaceholder->setVisible(u.listPlaceholderVisible);

    _ui.headerPanel->setVisible(u.headerVisible);
    _ui.headerPlaceholder->setVisible(u.headerPlaceholderVisible);

    _ui.itemTable->setVisible(u.itemTableVisible);
    _ui.itemPlaceholder->setVisible(u.itemPlaceholderVisible);

    if (_ui.itemToolbar)
        _ui.itemToolbar->setVisible(u.itemToolbarVisible);

    // LISTA TOOLBAR (header_list)
    if (auto* tb = this->findChild<QToolBar*>("order_header_list_toolbar")) {

        auto set = [&](const char* name, bool enabled) {
            if (auto* act = tb->findChild<QAction*>(name))
                act->setEnabled(enabled);
        };

        set("header_list_add",    u.headerAddEnabled);
        set("header_list_delete", u.headerDeleteEnabled);
        set("header_list_rename", u.headerModifyEnabled);
        set("header_list_refresh", true); // refresh mindig engedélyezett
    }

    // HEADER TOOLBAR (header)
    if (auto* tb = this->findChild<QToolBar*>("order_header_toolbar")) {

        auto set = [&](const char* name, bool enabled) {
            if (auto* act = tb->findChild<QAction*>(name))
                act->setEnabled(enabled);
        };

        set("header_modify", u.headerModifyEnabled);
        set("header_save",   u.headerSaveEnabled);
        set("header_cancel", u.headerCancelEnabled);
        set("header_delete", u.headerDeleteEnabled);
    }

    // ITEM TOOLBAR
    if (_ui.itemToolbar) {
        auto actions = _ui.itemToolbar->actions();
        if (actions.size() >= 2) {
            actions[0]->setEnabled(u.itemAddEnabled);
            actions[1]->setEnabled(u.itemDeleteEnabled);
        }
    }

    updateHeaderPlaceholderText(s);
    updateItemPlaceholderText(s);
}


void OrderWorkbench::updateHeaderPlaceholderText(State s)
{
    if (!_ui.headerPlaceholder)
        return;

    QString text;

    switch (s) {
    case State::InitialEmpty:
        text =
            "📭 Nincs még egyetlen rendelés sem\n\n"
            "Kattints a „+ Új” gombra az első rendelés létrehozásához.";
        break;

    case State::ListNoSelection:
        text =
            "📭 Nincs kiválasztott rendelés\n\n"
            "A bal oldali listából válassz egy rendelést,\n"
            "vagy kattints a „+ Új” gombra egy új rendelés létrehozásához.";
        break;

    case State::NewHeaderEditing:
        text =
            "📝 Új rendelés létrehozása folyamatban…";
        break;

    case State::HeaderSelected:
    case State::HeaderEditing:
        text = ""; // nem látszik, mert a panel látszik
        break;
    }

    if (auto* lbl = _ui.headerPlaceholder->findChild<QLabel*>())
        lbl->setText(text);
}

void OrderWorkbench::updateItemPlaceholderText(State s)
{
    if (!_ui.itemPlaceholder)
        return;

    QString text;

    switch (s) {
    case State::InitialEmpty:
        text =
            "📦 Nincsenek tételek\n\n"
            "Előbb hozz létre egy rendelést.";
        break;

    case State::ListNoSelection:
        text =
            "📦 Nincsenek tételek\n\n"
            "Válassz ki egy rendelést a bal oldali listából.";
        break;

    case State::NewHeaderEditing:
        text =
            "📦 Nincsenek tételek\n\n"
            "A tételek az új rendelés mentése után jelennek meg.";
        break;

    case State::HeaderSelected:
        text =
            "📦 Nincsenek tételek\n\n"
            "Adj hozzá új tételt a fenti gombbal.";
        break;

    case State::HeaderEditing:
        text =
            "📦 Tételszerkesztés folyamatban…";
        break;
    }

    if (auto* lbl = _ui.itemPlaceholder->findChild<QLabel*>())
        lbl->setText(text);
}

// bool OrderWorkbench::isHeaderModified() const
// {
//     // 1) Panel ID optional
//     auto panelIdOpt = _ui.headerPanel->headerId();
//     if (!panelIdOpt.has_value())
//         return false;   // nincs header → nincs mit módosítani

//     QUuid panelId = *panelIdOpt;

//     // 2) Eredeti header lekérése a registryből
//     auto originalOpt = OrderHeaderRegistry::instance().findById(panelId);
//     if (!originalOpt)
//         return false;   // új header → még nincs eredeti állapot

//     OrderHeader original = *originalOpt;

//     // 3) Jelenlegi header a panelből
//     OrderHeader current = _ui.headerPanel->header();  // NINCS paraméter!

//     // 4) Mezők összehasonlítása
//     return (
//         original.customerName != current.customerName ||
//         original.orderDate    != current.orderDate    ||
//         original.deadline     != current.deadline     ||
//         original.defaultExternalPrefix != current.defaultExternalPrefix ||
//         original.note         != current.note ||
//         original.status       != current.status
//         );
// }

bool OrderWorkbench::isHeaderModified() const
{
    // 1) Ha nincs panelId → nincs header → nem módosult
    auto panelIdOpt = _ui.headerPanel->headerId();
    if (!panelIdOpt.has_value()) {
        zInfo("isHeaderModified → false (nincs panelId)");
        return false;
    }

    // 2) Eredeti és aktuális header lekérése
    std::optional<OrderHeader> originalOpt = _ui.headerPanel->originalHeader();
    std::optional<OrderHeader> currentOpt  = _ui.headerPanel->header();

    // 3) Ha bármelyik hiányzik → nem módosult
    if (!originalOpt.has_value() || !currentOpt.has_value()) {
        zInfo("isHeaderModified → false (nincs original vagy current)");
        return false;
    }

    const OrderHeader& original = *originalOpt;
    const OrderHeader& current  = *currentOpt;

    bool modified = false;

    auto check = [&](const char* field, const auto& a, const auto& b) {
        if (a != b) {
            modified = true;
            zInfo(QString("isHeaderModified → %1 MISMATCH\n  original: %2\n  current:  %3")
                      .arg(field)
                      .arg(QVariant::fromValue(a).toString())
                      .arg(QVariant::fromValue(b).toString()));
        }
    };

    // 4) Mezőnkénti összehasonlítás
    check("customerName", original.customerName, current.customerName);
    check("orderDate", original.orderDate, current.orderDate);
    check("deadline", original.deadline, current.deadline);
    check("defaultExternalPrefix", original.defaultExternalPrefix, current.defaultExternalPrefix);
    check("note", original.note, current.note);
    check("status", original.status, current.status);

    // 5) Log
    if (!modified)
        zInfo("isHeaderModified → false (minden mező egyezik)");

    return modified;
}



QString OrderWorkbench::stateToString(State s) const
{
    switch (s) {
    case State::InitialEmpty: return "InitialEmpty";
    case State::NewHeaderEditing: return "NewHeaderEditing";
    case State::ListNoSelection: return "ListNoSelection";
    case State::HeaderSelected: return "HeaderSelected";
    case State::HeaderEditing: return "HeaderEditing";
    default: return "Unknown";
    }
}


