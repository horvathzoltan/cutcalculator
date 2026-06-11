#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>
#include <QStackedWidget>

#include <common/ui_state/i_workbench_custom_state.h>

#include <common/ui/crud/actionmap.h>

#include "common/ui/crud/list_toolbar_factory.h"
#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"
#include "orders/view/order_header_list_panel.h"
#include "orders/presenter/order_workbench_presenter.h"
#include "orders/manager/order_manager.h"
#include "workbench/view/order/state/header_editor_state_machine.h"
#include "workbench/view/order/state/item_state_machine.h"
#include "workbench/view/order/state/list_state_machine.h"
#include "workbench/view/order/order_workbench_ui_model.h"


class OrderWorkbench : public QWidget, public IWorkbenchCustomState  {
    Q_OBJECT
public:
    explicit OrderWorkbench(QWidget* parent = nullptr);



    struct UiState {
        bool listVisible;
        bool listPlaceholderVisible;

        bool headerVisible;
        bool headerPlaceholderVisible;

        bool itemTableVisible;
        bool itemPlaceholderVisible;
        bool itemToolbarVisible;

        bool headerAddEnabled;
        bool headerModifyEnabled;
        bool headerDeleteEnabled;
        bool headerSaveEnabled;
        bool headerCancelEnabled;

        bool itemAddEnabled;
        bool itemDeleteEnabled;
    };

private:
    OrderWorkbenchPresenter* _presenter = nullptr;
    OrderManager* _orderManager = nullptr;

    OrderWorkbenchUIModel _ui;

    void showHeaderPlaceholder(bool show);
    void showItemPlaceholder(bool show);

    // placeholder szöveg frissítése állapot alapján

    ListStateMachine listSM;
    HeaderEditorStateMachine headerSM;
    ItemStateMachine itemSM;

    bool isHeaderModified() const;
public:
    QVariantMap saveCustomState() const override;
    void restoreCustomState(const QVariantMap& state) override;

    Q_INVOKABLE void postRestoreFix();
    void updateUIState();
};
