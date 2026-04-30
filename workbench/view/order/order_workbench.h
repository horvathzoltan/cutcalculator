#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>

#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"
#include "orders/view/order_header_list_panel.h"
#include "orders/presenter/order_workbench_presenter.h"
#include "orders/manager/order_manager.h"
#include "orders/repository/order_header_repository.h"
#include "orders/repository/order_item_repository.h"



class OrderWorkbench : public QWidget {
    Q_OBJECT
public:
    explicit OrderWorkbench(QWidget* parent = nullptr);

private:
    OrderWorkbenchPresenter* _presenter = nullptr;
    OrderManager* _orderManager = nullptr;

    // Layout
    QVBoxLayout* _layout = nullptr;
    QSplitter* _splitter = nullptr;
    QSplitter* _rightSplitter = nullptr;

    OrderHeaderListPanel* _listPanel = nullptr;
    OrderHeaderPanel* _headerPanel = nullptr;
    OrderItemTable* _itemTable = nullptr;

    // Toolbar
    QToolBar* _toolbar = nullptr;

    // Builders
    void buildToolbar();
    void buildPanels();
    // protected:
    //     void hideEvent(QHideEvent* e) override;



};
