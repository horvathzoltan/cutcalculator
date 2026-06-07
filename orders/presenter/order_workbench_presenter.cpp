#include "orders/presenter/order_workbench_presenter.h"
#include <QMessageBox>
#include "common/logger/event_logger.h"
//#include "common/ui/crud/crud_toolbar_factory.h"
//#include "orders/registry/order_item_registry.h"
#include "workbench/view/order/order_workbench.h"
#include <common/ui_state/workbench_state_manager.h>
//#include "orders/registry/order_header_registry.h"
#include <common/ui/crud/entity_toolbar_factory.h>
#include <common/ui/crud/list_toolbar_factory.h>

OrderWorkbenchPresenter::OrderWorkbenchPresenter(OrderHeaderPanel* headerPanel,
                                                 OrderItemTable* itemTable,
                                                 OrderManager* manager,
                                                 OrderHeaderListPanel* listPanel,
                                                 RepositoryOverlayWidget<OrderHeaderRegistry>* headerOverlay,
                                                 RepositoryOverlayWidget<OrderItemRegistry>* itemOverlay,
                                                 QObject* parent)
    : QObject(parent),
    _headerPanel(headerPanel),
    _itemTable(itemTable),
    _manager(manager),
    _listPanel(listPanel),
    _itemOverlay(itemOverlay),
    _headerOverlay(headerOverlay)
{

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
            this, &OrderWorkbenchPresenter::onItemTableChanged);



    // ÚJ: HeaderPanel mezőváltozás → Workbench állapot frissítése
    if (auto* wb = qobject_cast<OrderWorkbench*>(parent)) {

        // customerName
        connect(_headerPanel->customerNameEdit(), &QLineEdit::textChanged,
                wb, &OrderWorkbench::updateUIState);


        // deadline
        connect(_headerPanel->deadlineEdit(), &QDateEdit::dateChanged,
                wb, &OrderWorkbench::updateUIState);

        // defaultExternalPrefix
        connect(_headerPanel->defaultExternalPrefixEdit(), &QLineEdit::textChanged,
                wb, &OrderWorkbench::updateUIState);

        // note
        connect(_headerPanel->noteEdit(), &QTextEdit::textChanged,
                wb, &OrderWorkbench::updateUIState);
    }

}

void OrderWorkbenchPresenter::refreshListAndSelect(const QUuid& id)
{
    if (!_listPanel)
        return;

    _listPanel->refresh();
    _listPanel->selectById(id);

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();

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

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();

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

    // 1) HeaderPanel feltöltése
    _headerPanel->setHeader(h);

    // 2) ItemTable ürítése
    _itemTable->setRowCount(0);

    // 3) Lista frissítése és kiválasztás
    _listPanel->refresh();
    _listPanel->selectById(_currentOrderId);

    // 4) UI állapot frissítése
    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();

    // 5) Overlay frissítések
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

    _headerPanel->setHeader(*headerOpt);

    QVector<OrderItem> items = _manager->loadItems(id);
    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();

    _itemTable->setItems(items);

    if (_headerOverlay)
        _headerOverlay->refresh(_listPanel->visibleRowCount());

}

void OrderWorkbenchPresenter::saveOrder()
{
    // 1) Header lekérése (optional!)
    std::optional<OrderHeader> hOpt = _headerPanel->header();
    if (!hOpt) {
        zWarning("saveOrder → nincs header, mentés kihagyva");
        return;
    }

    OrderHeader h = *hOpt;

    // 2) Itemek lekérése
    QVector<OrderItem> items = _itemTable->toItems(_currentOrderId, h.customerName);

    // 3) Mentés
    if (!_manager->saveOrder(h, items)) {
        zWarning("saveOrder → mentés sikertelen");
        return;
    }

    // 4) Sikeres mentés → lista frissítése + kiválasztás + overlay + UI state
    refreshListAndSelect(h.id);

    zInfo("saveOrder → sikeres mentés, refreshListAndSelect futott");
}

void OrderWorkbenchPresenter::deleteCurrentOrder()
{
    if (_currentOrderId.isNull())
        return;

    zEventWARN("Header: Rendelés törlése indult");

    _manager->deleteOrder(_currentOrderId);

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();
}

void OrderWorkbenchPresenter::renameCurrentOrder()
{
    auto idOpt = _listPanel->selectedOrderId();
    if (!idOpt)
        return;

    loadOrder(*idOpt);   // átnevezés = betöltés szerkesztésre
}

void OrderWorkbenchPresenter::addItem()
{
    // 0) UX védelem: nincs kiválasztott order → nem engedjük
    if (_currentOrderId.isNull()) {

        zEventWARN("Item: Új tétel gomb megnyomva, de nincs kiválasztott rendelés");

        QMessageBox::information(
            nullptr,
            "Nincs rendelés",
            "Előbb hozz létre vagy válassz ki egy rendelést.\n"
            "Tételeket csak meglévő rendeléshez adhatsz hozzá."
            );

        return;   // <-- KÖTELEZŐ!
    }

    zEventINFO("Item: Új tétel hozzáadása indult");

    // 1) Header lekérése (optional!)
    std::optional<OrderHeader> hOpt = _headerPanel->header();
    if (!hOpt) {
        zEventWARN("Item: nincs header a panelen, nem lehet tételt hozzáadni");
        return;
    }

    OrderHeader h = *hOpt;

    // 2) Jelenlegi tételek
    QVector<OrderItem> items = _itemTable->toItems(_currentOrderId, h.customerName);

    // 3) Új tétel létrehozása
    OrderItem it;
    it.id = QUuid::createUuid();
    it.orderId = _currentOrderId;
    it.ownerName = h.customerName;
    it.order_qty = 1;

    items.append(it);

    // 4) UI frissítés
    _itemTable->setItems(items);

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();

    // 5) Scroll az utolsó sorra
    _itemTable->scrollToBottom();

    zEventINFO("Item: új tétel sikeresen hozzáadva");
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

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();

    if (_itemOverlay)
        _itemOverlay->refresh(_itemTable->rowCount());
}

void OrderWorkbenchPresenter::refreshHeaderOverlay()
{
    if (_headerOverlay && _listPanel)
        _headerOverlay->refresh(_listPanel->visibleRowCount());
}

void OrderWorkbenchPresenter::onItemTableChanged()
{
    if (_itemOverlay)
        _itemOverlay->refresh(_itemTable->rowCount());

    if (auto* wb = qobject_cast<OrderWorkbench*>(parent()))
        wb->updateUIState();
}

