#include "order_workbench_ui_builder.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>

#include "workbench/view/order/order_workbench.h"
#include <common/ui/crud/list_toolbar_factory.h>
#include <common/ui/crud/entity_toolbar_factory.h>

OrderWorkbench::UIModel
OrderWorkbenchUIBuilder::build(OrderWorkbench* wb)
{
    OrderWorkbench::UIModel ui;

    // MAIN LAYOUT + SPLITTER
    auto* layout = new QVBoxLayout(wb);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* mainSplitter = new QSplitter(Qt::Horizontal, wb);
    mainSplitter->setObjectName("order_main_splitter");
    layout->addWidget(mainSplitter);

    // LIST CONTAINER
    ui.listContainer = new QWidget(mainSplitter);
    ui.listContainer->setMinimumWidth(250);

    auto* lcLayout = new QVBoxLayout(ui.listContainer);
    lcLayout->setContentsMargins(0, 0, 0, 0);

    // LIST PANEL
    ui.listPanel = new OrderHeaderListPanel(ui.listContainer);
    ui.listPanel->setObjectName("order_header_list_panel");
    ui.listPanel->setMinimumWidth(250);

    // LIST PLACEHOLDER
    ui.listPlaceholder = new QWidget(ui.listContainer);
    ui.listPlaceholder->setMinimumWidth(250);

    auto* lpLayout = new QVBoxLayout(ui.listPlaceholder);
    lpLayout->setAlignment(Qt::AlignCenter);

    auto* lpLabel = new QLabel(
        "📭 Még nincs egyetlen rendelés sem\n\n"
        "Kattints a „+ Új” gombra az első rendelés létrehozásához.",
        ui.listPlaceholder
        );
    lpLabel->setAlignment(Qt::AlignCenter);
    lpLabel->setStyleSheet("font-size: 16px; color: #666;");
    lpLayout->addWidget(lpLabel);

    ui.listPlaceholder->show();
    ui.listPanel->hide();

    lcLayout->addWidget(ui.listPlaceholder);
    lcLayout->addWidget(ui.listPanel);

    // RIGHT SPLITTER
    auto* rightSplitter = new QSplitter(Qt::Vertical, mainSplitter);
    rightSplitter->setObjectName("order_right_splitter");

    // HEADER CONTAINER
    ui.headerContainer = new QWidget(rightSplitter);
    auto* headerLayout = new QVBoxLayout(ui.headerContainer);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    ui.headerPanel = new OrderHeaderPanel(ui.headerContainer);
    ui.headerPanel->setObjectName("order_header_panel");

    ui.headerPlaceholder = new QWidget(ui.headerContainer);
    auto* phLayout = new QVBoxLayout(ui.headerPlaceholder);
    phLayout->setAlignment(Qt::AlignCenter);

    QString headerText;
    auto* phLabel = new QLabel(headerText, ui.headerPlaceholder);
    phLabel->setAlignment(Qt::AlignCenter);
    phLabel->setStyleSheet("font-size: 16px; color: #666;");
    phLayout->addWidget(phLabel);

    ui.headerPlaceholder->show();
    ui.headerPanel->hide();

    headerLayout->addWidget(ui.headerPlaceholder);
    headerLayout->addWidget(ui.headerPanel);

    // ITEM CONTAINER
    ui.itemContainer = new QWidget(rightSplitter);

    ui.itemTable = new OrderItemTable(ui.itemContainer);
    ui.itemTable->setObjectName("order_item_table");

    ui.itemPlaceholder = new QWidget(ui.itemContainer);
    auto* iphLayout = new QVBoxLayout(ui.itemPlaceholder);
    iphLayout->setAlignment(Qt::AlignCenter);

    QString itemText;
    auto* iphLabel = new QLabel(itemText, ui.itemPlaceholder);
    iphLabel->setAlignment(Qt::AlignCenter);
    iphLabel->setStyleSheet("font-size: 16px; color: #666;");
    iphLayout->addWidget(iphLabel);

    ui.itemPlaceholder->show();
    ui.itemTable->hide();

    auto* itemLayout = new QVBoxLayout(ui.itemContainer);
    itemLayout->setContentsMargins(0, 0, 0, 0);
    itemLayout->addWidget(ui.itemPlaceholder);
    itemLayout->addWidget(ui.itemTable);
    ui.itemContainer->setLayout(itemLayout);

    rightSplitter->addWidget(ui.headerContainer);
    rightSplitter->addWidget(ui.itemContainer);

    // LIST TOOLBAR (HEADER LIST)
    {
        ListToolbarConfig cfg;
        cfg.parent = ui.listContainer;
        cfg.objectNamePrefix = "header_list";
        cfg.labelPrefix = "rendelés";

        cfg.actions = {
            ListAction::Add,
            ListAction::Delete,
            ListAction::Rename,
            ListAction::Refresh
        };

        // Callbacks a Presenter fogja később beállítani (itt csak UI-t építünk)

        ListToolbarResult tb1 =
            ListToolbarFactory::create<OrderHeaderRegistry>(cfg);        
        tb1.toolbar->setObjectName("order_header_list_toolbar");
        lcLayout->insertWidget(0, tb1.toolbar);

        ui.listActions = tb1.actionPointers;
        ui.headerOverlay = static_cast<RepositoryOverlayWidget<OrderHeaderRegistry>*>(tb1.overlay);
    }

    // HEADER TOOLBAR
    {
        EntityToolbarConfig cfg;
        cfg.parent = ui.headerContainer;
        cfg.objectNamePrefix = "header";
        cfg.labelPrefix = "rendelés";

        cfg.actions = {
            EntityAction::Modify,
            EntityAction::Save,
            EntityAction::Cancel,
            EntityAction::Delete
        };

        auto tb1 = EntityToolbarFactory::create<OrderHeaderRegistry>(cfg);
        //ui.headerToolbar = tb1.toolbar;
        tb1.toolbar->setObjectName("order_header_toolbar");

        if (auto* headerLayout = qobject_cast<QVBoxLayout*>(ui.headerContainer->layout())) {
            headerLayout->insertWidget(0, tb1.toolbar);
        }
    }

    // ITEM TOOLBAR
    {
        EntityToolbarConfig cfg;
        cfg.parent = ui.itemContainer;
        cfg.objectNamePrefix = "item";
        cfg.labelPrefix = "tétel";

        cfg.actions = {
            EntityAction::Add,
            EntityAction::Delete
        };

        auto tb1 = EntityToolbarFactory::create<OrderItemRegistry>(cfg);
        ui.itemOverlay = static_cast<RepositoryOverlayWidget<OrderItemRegistry>*>(tb1.overlay);
        ui.itemToolbar = tb1.toolbar;
        ui.itemToolbar->setObjectName("order_item_toolbar");

        if (auto* itemLayout = qobject_cast<QVBoxLayout*>(ui.itemContainer->layout())) {
            itemLayout->insertWidget(0, ui.itemToolbar);
        }
    }

    return ui;
}
