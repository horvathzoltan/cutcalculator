#include "order_workbench.h"
#include "common/ui_state/widget_state_manager.h"
#include <QTimer>
#include <common/ui_state/workbench_state_manager.h>

OrderWorkbench::OrderWorkbench(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("order_workbench");

    // ⭐ REGISTER WORKBENCH
    WorkbenchStateManager::instance().registerWorkbench("order_workbench", this);

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    buildToolbar();

    auto newAction = _toolbar->actions()[0];
    auto saveAction = _toolbar->actions()[1];
    auto loadAction = _toolbar->actions()[2];
    auto deleteAction = _toolbar->actions()[3];

    connect(newAction, &QAction::triggered, this, [this]() {
        _presenter->newOrder();
    });

    connect(saveAction, &QAction::triggered, this, [this]() {
        _presenter->saveOrder();
    });

    connect(loadAction, &QAction::triggered, this, [this]() {
        QUuid id = _listPanel->selectedOrderId();
        if (!id.isNull())
            _presenter->loadOrder(id);
    });

    connect(deleteAction, &QAction::triggered, this, [this]() {
        _presenter->deleteOrder();
    });

    connect(_listPanel, &OrderHeaderListPanel::orderSelected,
            this, [this](const QUuid& id) {
                _presenter->loadOrder(id);
            });

    _layout->addWidget(_toolbar);

    // Splitter
    _splitter = new QSplitter(Qt::Horizontal, this);
    _splitter->setObjectName("order_main_splitter");
    _layout->addWidget(_splitter);

    _listPanel = new OrderHeaderListPanel(_splitter);
    _listPanel->setObjectName("order_header_list_panel");

    _rightSplitter = new QSplitter(Qt::Vertical, _splitter);
    _rightSplitter->setObjectName("order_right_splitter");

    buildPanels();

    _splitter->addWidget(_listPanel);
    _splitter->addWidget(_rightSplitter);

    _orderManager = new OrderManager(this);

    _presenter = new OrderWorkbenchPresenter(
        _headerPanel,
        _itemTable,
        _orderManager,
        _listPanel,
        this
        );

}

void OrderWorkbench::buildToolbar()
{
    _toolbar = new QToolBar("Order Actions", this);
    _toolbar->setObjectName("order_toolbar");

    _toolbar->addAction("New");
    _toolbar->addAction("Save");
    _toolbar->addAction("Load");
    _toolbar->addAction("Delete");
}

void OrderWorkbench::buildPanels()
{
    _headerPanel = new OrderHeaderPanel(_rightSplitter);
    _headerPanel->setObjectName("order_header_panel");

    _itemTable = new OrderItemTable(_rightSplitter);
    _itemTable->setObjectName("order_item_table");

    _rightSplitter->addWidget(_headerPanel);
    _rightSplitter->addWidget(_itemTable);
}



// void OrderWorkbench::hideEvent(QHideEvent* e)
// {
//     // WorkbenchStateManager::instance().onTabDeactivated(this);
//     // QWidget::hideEvent(e);
// }
