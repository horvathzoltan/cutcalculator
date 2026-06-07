#pragma once
#include <QWidget>
#include <QToolBar>

#include "common/ui/crud/list_toolbar_factory.h"
#include "orders/view/order_header_panel.h"
#include "orders/view/order_item_table.h"
#include "orders/view/order_header_list_panel.h"
#include "ui/helpers/repository_overlay_widget.h"
#include "orders/registry/order_header_registry.h"
#include "orders/registry/order_item_registry.h"
#include "workbench/view/order/order_workbench.h"

//class OrderWorkbench;

class OrderWorkbenchUIBuilder
{
public:

    // UIModel = logikai UI elemek, amelyeket a Workbench vagy a Presenter használ.
    // Nem tartalmaz pusztán vizuális widgeteket (toolbar, layout, splitter).

    static OrderWorkbench::UIModel build(OrderWorkbench* wb);
};
