#pragma once
#include <QObject>
#include <QUuid>

#include "orders/model/order_header.h"
#include "orders/model/order_item.h"
#include "orders/manager/order_manager.h"
#include "orders/view/order_header_list_panel.h"
#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"

class OrderWorkbenchPresenter : public QObject {
    Q_OBJECT
public:
    explicit OrderWorkbenchPresenter(OrderHeaderPanel* headerPanel,
                                     OrderItemTable* itemTable,
                                     OrderManager* manager,
                                     OrderHeaderListPanel* listPanel,
                                     QObject* parent);

    void newOrder();
    void loadOrder(const QUuid& id);
    void saveOrder();
    void deleteOrder();
    void refreshListAndSelect(const QUuid& id);

private:
    OrderHeaderPanel* _headerPanel = nullptr;
    OrderItemTable* _itemTable = nullptr;
    OrderManager* _manager = nullptr;
    OrderHeaderListPanel* _listPanel = nullptr;

    QUuid _currentOrderId;
};
