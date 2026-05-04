#include "orders/presenter/order_workbench_presenter.h"
#include <QMessageBox>
#include "common/logger/event_logger.h"
#include "common/ui/crud/crud_toolbar_factory.h"
#include "orders/registry/order_item_registry.h"
#include "workbench/view/order/order_workbench.h"

OrderWorkbenchPresenter::OrderWorkbenchPresenter(OrderHeaderPanel* headerPanel,
                                                 OrderItemTable* itemTable,
                                                 OrderManager* manager,
                                                 OrderHeaderListPanel* listPanel,
                                                 QObject* parent)
    : QObject(parent),
    _headerPanel(headerPanel),
    _itemTable(itemTable),
    _manager(manager),
    _listPanel(listPanel)
{
    newOrder();

    // Manager jelek → Presenter
    connect(_manager, &OrderManager::orderSaved,
            this, &OrderWorkbenchPresenter::refreshListAndSelect);

    connect(_manager, &OrderManager::orderDeleted,
            this, &OrderWorkbenchPresenter::refreshListAndSelectNextAfter);

    connect(_manager, &OrderManager::validationFailed,
            this, [this](const QString& msg) {
                QMessageBox::warning(nullptr, "Validation error", msg);
            });

    connect(_itemTable, &QTableWidget::itemChanged,
            this, [this]() {
                if (_itemOverlay)
                    _itemOverlay->refresh(_itemTable->rowCount());
            });

}

void OrderWorkbenchPresenter::refreshListAndSelect(const QUuid& id)
{
    if (!_listPanel)
        return;

    _listPanel->refresh();      // ⭐ publikus API
    _listPanel->selectById(id); // ⭐ publikus API

    if (_headerOverlay)
        _headerOverlay->refresh(_listPanel->visibleRowCount());
}

void OrderWorkbenchPresenter::refreshListAndSelectNextAfter(const QUuid& deletedId)
{
    if (!_listPanel)
        return;

    _listPanel->refresh();

    QUuid nextId = _listPanel->nextOrderIdAfter(deletedId);
    if (!nextId.isNull()) {
        _listPanel->selectById(nextId);
        loadOrder(nextId);
    } else {
        _headerPanel->clear();
        _itemTable->setRowCount(0);
    }

    if (_itemOverlay && _itemTable)
        _itemOverlay->refresh(_itemTable->rowCount());

    if (_headerOverlay)
        _headerOverlay->refresh(_listPanel->visibleRowCount());
}


void OrderWorkbenchPresenter::newOrder()
{
    zEventINFO("Header: Új rendelés létrehozása");
    _currentOrderId = QUuid::createUuid();

    OrderHeader h;
    h.id = _currentOrderId;
    h.customerName = "";
    h.orderDate = QDate::currentDate();
    h.deadline = QDate::currentDate();
    h.defaultExternalPrefix = "";
    h.note = "";
    h.status = "NEW";

    showHeaderPlaceholder(false);
    _headerPanel->setHeader(h);

    _itemTable->setRowCount(0);

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->showItemPlaceholder(true);


    if (_itemOverlay)
        _itemOverlay->refresh(0);

    if (_headerOverlay)
        _headerOverlay->refresh(_listPanel->visibleRowCount());

}

void OrderWorkbenchPresenter::loadOrder(const QUuid& id)
{
    std::optional<OrderHeader> headerOpt = _manager->loadHeader(id);
    if (!headerOpt)
        return;

    _currentOrderId = id;
    showHeaderPlaceholder(false);
    _headerPanel->setHeader(*headerOpt);

    QVector<OrderItem> items = _manager->loadItems(id);
    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->showItemPlaceholder(items.isEmpty());

    _itemTable->setItems(items);

}

void OrderWorkbenchPresenter::saveOrder()
{
    OrderHeader h = _headerPanel->toHeader(_currentOrderId);
    QVector<OrderItem> items = _itemTable->toItems(_currentOrderId, h.customerName);

    if (!_manager->saveOrder(h, items)) {
        return;
    }

    // Sikeres mentés → lista frissítése (PATCH 17)
}

void OrderWorkbenchPresenter::deleteOrder()
{
    if (_currentOrderId.isNull())
        return;

    zEventWARN("Header: Rendelés törlése indult");

    _manager->deleteOrder(_currentOrderId);

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->showItemPlaceholder(true);
}



void OrderWorkbenchPresenter::addItem()
{
    // 🔥 UX védelem: nincs kiválasztott order → nem engedjük
    if (_currentOrderId.isNull()) {

        // 🔥 User event log (domain-szintű)
        // USER EVENT LOG (UI-ra is megy)
        zEventWARN("Item: Új tétel gomb megnyomva, de nincs kiválasztott rendelés");

        QMessageBox::information(
            nullptr,
            "Nincs rendelés",
            "Előbb hozz létre vagy válassz ki egy rendelést.\n"
            "Tételeket csak meglévő rendeléshez adhatsz hozzá."
            );
    }

    // 🔥 User event log: sikeres művelet
    zEventINFO("Item: Új tétel hozzáadása indult");

    // 1) Header kell az ownerName miatt
    OrderHeader h = _headerPanel->toHeader(_currentOrderId);

    // 2) Jelenlegi tételek
    QVector<OrderItem> items = _itemTable->toItems(_currentOrderId, h.customerName);

    // 3) Új tétel
    OrderItem it;
    it.id = QUuid::createUuid();
    it.orderId = _currentOrderId;
    it.ownerName = h.customerName;
    it.order_qty = 1;

    items.append(it);

    // 4) UI frissítés
    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->showItemPlaceholder(false);

    _itemTable->setItems(items);

    // 5) Scroll az utolsó sorra
    _itemTable->scrollToBottom();
}

void OrderWorkbenchPresenter::deleteItem()
{
    if (_currentOrderId.isNull())
        return;

    auto items = _itemTable->selectedItems();
    if (items.isEmpty())
        return;

    int row = items.first()->row();

    // ID lekérése
    QVariant idv = _itemTable->item(row, 0)->data(Qt::UserRole + 100);
    if (!idv.isValid())
        return;

    QUuid itemId = idv.toUuid();
    if (itemId.isNull())
        return;

    // Domain törlés
    _manager->deleteItem(itemId);

    // UI frissítés
    QVector<OrderItem> refreshed = _manager->loadItems(_currentOrderId);
    _itemTable->setItems(refreshed);
}

QToolBar* OrderWorkbenchPresenter::buildItemToolbar(QWidget* parent)
{
    CrudToolbarConfig cfg;
    cfg.parent = parent;
    cfg.actions = { CrudAction::Add, CrudAction::Delete };
    cfg.callbacks.onAdd    = [this]() { addItem(); };
    cfg.callbacks.onDelete = [this]() { deleteItem(); };
    cfg.overlay = CrudOverlay::Enabled;

    // 🔥 ÚJ: a Factory most CrudToolbarResult‑ot ad vissza
    auto r = CrudToolbarFactory::create<OrderItemRegistry>(cfg);

    // 🔥 eltesszük az overlay pointert
    _itemOverlay = static_cast<RepositoryOverlayWidget<OrderItemRegistry>*>(r.overlay);

    // 🔥 első refresh (ha már van táblázat)
    if (_itemOverlay && _itemTable)
        _itemOverlay->refresh(_itemTable->rowCount());

    r.toolbar->setObjectName("order_item_toolbar");
    return r.toolbar;
}

QToolBar* OrderWorkbenchPresenter::buildHeaderToolbar(QWidget* parent)
{
    CrudToolbarConfig cfg;
    cfg.parent = parent;
    cfg.actions = {
        CrudAction::Add,
        CrudAction::Delete,
        CrudAction::Rename
        // Clone opcionális
    };

    cfg.callbacks.onAdd = [this]() { newOrder(); };
    cfg.callbacks.onDelete = [this]() { deleteOrder(); };
    //cfg.callbacks.onRename = [this]() { renameOrder(); }; // ha kell
    cfg.overlay = CrudOverlay::Enabled;

    auto r = CrudToolbarFactory::create<OrderHeaderRegistry>(cfg);

    _headerOverlay = static_cast<RepositoryOverlayWidget<OrderHeaderRegistry>*>(r.overlay);

    r.toolbar->setObjectName("order_header_toolbar");
    return r.toolbar;
}

void OrderWorkbenchPresenter::refreshHeaderOverlay()
{
    if (_headerOverlay && _listPanel)
        _headerOverlay->refresh(_listPanel->visibleRowCount());
}


void OrderWorkbenchPresenter::showHeaderPlaceholder(bool show)
{
    if (!_headerPanel || !_listPanel)
        return;

    if (show) {
        _headerPanel->hide();
        if (auto* wb = qobject_cast<OrderWorkbench*>(parent())) {
            wb->showHeaderPlaceholder(true);
        }

    } else {
        if (auto* wb = qobject_cast<OrderWorkbench*>(parent())) {
            wb->showHeaderPlaceholder(false);
        }

        _headerPanel->show();
    }
}
