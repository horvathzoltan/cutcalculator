#include "orders/presenter/order_workbench_presenter.h"

#include <QMessageBox>

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
