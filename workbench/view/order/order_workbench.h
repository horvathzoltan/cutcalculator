#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>

#include <common/ui_state/i_workbench_custom_state.h>

#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"
#include "orders/view/order_header_list_panel.h"
#include "orders/presenter/order_workbench_presenter.h"
#include "orders/manager/order_manager.h"
#include "orders/repository/order_header_repository.h"
#include "orders/repository/order_item_repository.h"



class OrderWorkbench : public QWidget, public IWorkbenchCustomState  {
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

    QToolBar* _itemToolbar = nullptr;
    // régi item toolbar actionök eltávolítva

    // Builders
    void buildToolbar();
    void buildPanels();
    // protected:
    //     void hideEvent(QHideEvent* e) override;


public:
    QVariantMap saveCustomState() const override;
    void restoreCustomState(const QVariantMap& state) override;

};
