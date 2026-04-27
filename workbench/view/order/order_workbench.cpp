#include "order_workbench.h"
#include <QTimer>
#include <common/ui_state/workbench_state_manager.h>

OrderWorkbench::OrderWorkbench(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("order_workbench");

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    buildToolbar();
    _layout->addWidget(_toolbar);

    // Splitter
    _splitter = new QSplitter(Qt::Vertical, this);
    _splitter->setObjectName("order_main_splitter");
    _layout->addWidget(_splitter);

    // Panels
    buildPanels();
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
    _headerPanel = new OrderHeaderPanel(_splitter);
    _headerPanel->setObjectName("order_header_panel");

    _itemTable = new OrderItemTable(_splitter);
    _itemTable->setObjectName("order_item_table");

    _splitter->addWidget(_headerPanel);
    _splitter->addWidget(_itemTable);
}


// void OrderWorkbench::hideEvent(QHideEvent* e)
// {
//     // WorkbenchStateManager::instance().onTabDeactivated(this);
//     // QWidget::hideEvent(e);
// }
