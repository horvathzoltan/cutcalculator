#include "order_workbench.h"
#include "common/ui_state/widget_state_manager.h"
#include <QTimer>

OrderWorkbench::OrderWorkbench(QWidget* parent)
    : QWidget(parent)
{
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

void OrderWorkbench::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (_restoredOnce) return;

    QTimer::singleShot(0, this, [this]() {
        _restoredOnce = true;
        _isFullyShown = true;
        tryRestore();
    });
}

void OrderWorkbench::tryRestore()
{
    if (!_isFullyShown || !_canRestore)
        return;

    restoreUiState();
}

void OrderWorkbench::restoreUiState()
{
    WidgetStateManager c("order_workbench");
    c.restoreWidgetState(this);
}

void OrderWorkbench::saveUiState()
{
    WidgetStateManager c("order_workbench");
    c.saveWidgetState(this);
}
