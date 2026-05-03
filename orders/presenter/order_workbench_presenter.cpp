#include "orders/presenter/order_workbench_presenter.h"
#include <QMessageBox>
#include "common/ui/crud/crud_toolbar_factory.h"
#include "orders/registry/order_item_registry.h"

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
            this, &OrderWorkbenchPresenter::refreshListAndSelect);

    connect(_manager, &OrderManager::validationFailed,
            this, [this](const QString& msg) {
                QMessageBox::warning(nullptr, "Validation error", msg);
            });

}

void OrderWorkbenchPresenter::refreshListAndSelect(const QUuid& id)
{
    if (!_listPanel)
        return;

    _listPanel->refresh();      // ⭐ publikus API
    _listPanel->selectById(id); // ⭐ publikus API
}



void OrderWorkbenchPresenter::newOrder()
{
    _currentOrderId = QUuid::createUuid();

    OrderHeader h;
    h.id = _currentOrderId;
    h.customerName = "";
    h.orderDate = QDate::currentDate();
    h.deadline = QDate::currentDate();
    h.defaultExternalPrefix = "";
    h.note = "";
    h.status = "NEW";

    _headerPanel->setHeader(h);
    _itemTable->setRowCount(0);
}

void OrderWorkbenchPresenter::loadOrder(const QUuid& id)
{
    std::optional<OrderHeader> headerOpt = _manager->loadHeader(id);
    if (!headerOpt)
        return;

    _currentOrderId = id;
    _headerPanel->setHeader(*headerOpt);

    QVector<OrderItem> items = _manager->loadItems(id);
    _itemTable->setItems(items);
}

void OrderWorkbenchPresenter::saveOrder()
{
    OrderHeader h = _headerPanel->toHeader(_currentOrderId);
    QVector<OrderItem> items = _itemTable->toItems(_currentOrderId, h.customerName);

    _manager->saveOrder(h, items);

    if (!_manager->saveOrder(h, items)) {
        // validationFailed jelzés már megtörtént
        return;
    }

    // Sikeres mentés → lista frissítése (PATCH 17)

}

void OrderWorkbenchPresenter::deleteOrder()
{
    _manager->deleteOrder(_currentOrderId);
    newOrder();
}

void OrderWorkbenchPresenter::addItem()
{
    if (_currentOrderId.isNull())
        return;

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

// QToolBar* OrderWorkbenchPresenter::buildItemToolbar(QWidget* parent)
// {
//     auto* tb = new QToolBar(parent);

//     QAction* addAct    = tb->addAction(QStringLiteral("➕ Új tétel"));
//     QAction* removeAct = tb->addAction(QStringLiteral("🗑️ Tétel törlése"));

//     QObject::connect(addAct, &QAction::triggered, this, [this]() {
//         addItem();
//     });

//     QObject::connect(removeAct, &QAction::triggered, this, [this]() {
//         deleteItem();
//     });

//     return tb;
// }





QToolBar* OrderWorkbenchPresenter::buildItemToolbar(QWidget* parent)
{
    CrudToolbarConfig cfg;
    cfg.parent = parent;
    cfg.actions = { CrudAction::Add, CrudAction::Delete };
    cfg.callbacks.onAdd    = [this]() { addItem(); };
    cfg.callbacks.onDelete = [this]() { deleteItem(); };
    cfg.overlay = CrudOverlay::Enabled;

    auto* tb = CrudToolbarFactory::create<OrderItemRegistry>(cfg);
    tb->setObjectName("order_item_toolbar");
    return tb;
}

