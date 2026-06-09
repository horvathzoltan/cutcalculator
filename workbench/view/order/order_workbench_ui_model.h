#pragma once

#include <QWidget>
#include <QToolBar>
#include <common/ui/crud/actionmap.h>
#include <ui/helpers/repository_overlay_widget.h>
#include "common/ui/crud/entity_toolbar_factory.h"
#include "common/ui/crud/list_toolbar_factory.h"
#include "common/ui/placeholder_widget.h"
#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"
#include "orders/view/order_header_list_panel.h"
#include "orders/registry/order_header_registry.h"
#include "orders/registry/order_item_registry.h"

struct OrderWorkbenchUIModel {
    QWidget* listContainer;
    QWidget* headerContainer;
    QWidget* itemContainer;

    OrderHeaderListPanel* listPanel;
    OrderHeaderPanel* headerPanel;
    OrderItemTable* itemTable;

    PlaceholderWidget* listPlaceholder;
    QWidget* headerPlaceholder;
    QWidget* itemPlaceholder;


    RepositoryOverlayWidget<OrderHeaderRegistry>* headerOverlay;
    RepositoryOverlayWidget<OrderItemRegistry>* itemOverlay;

    QToolBar* listToolBar;
    ActionMap<ListAction> listActions;

    QToolBar* itemToolbar;
    ActionMap<EntityAction> itemActions;

    QToolBar* headerToolbar;
    ActionMap<EntityAction> headerActions;
};
