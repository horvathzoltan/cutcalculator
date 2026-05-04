#pragma once
#include <QObject>
#include <QToolBar>
#include <QUuid>

#include <ui/helpers/repository_overlay_widget.h>

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

    void addItem();
    void deleteItem();
    QToolBar* buildItemToolbar(QWidget* parent);
    QToolBar* buildHeaderToolbar(QWidget* parent);

    void refreshHeaderOverlay();

    void showHeaderPlaceholder(bool show);

private:
    OrderHeaderPanel* _headerPanel = nullptr;
    OrderItemTable* _itemTable = nullptr;
    OrderManager* _manager = nullptr;
    OrderHeaderListPanel* _listPanel = nullptr;    
    RepositoryOverlayWidget<OrderItemRegistry>* _itemOverlay = nullptr;
    RepositoryOverlayWidget<OrderHeaderRegistry>* _headerOverlay = nullptr;

    QUuid _currentOrderId;
    void refreshListAndSelectNextAfter(const QUuid &deletedId);
};
