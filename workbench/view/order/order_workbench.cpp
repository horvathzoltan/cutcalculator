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


void OrderWorkbench::updateUIState()
{
    auto& registry = OrderHeaderRegistry::instance();
    bool modified = isHeaderModified();

    // HEADER
    auto h = HeaderEditorStateMachine::resolve(_ui, registry, modified);
    HeaderEditorStateMachine::apply(_ui, h);

    // LIST
    auto l = ListStateMachine::resolve(_ui, registry);
    ListStateMachine::apply(_ui, l);

    // ITEM
    auto i = ItemStateMachine::resolve(_ui, registry);
    ItemStateMachine::apply(_ui, i);
}



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




