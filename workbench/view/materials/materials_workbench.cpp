#include "materials_workbench.h"
#include "common/logger/logger.h"
#include "materials/view/material_table_widget.h"
#include "materials/view/material_table_manager.h"

#include <QVBoxLayout>
#include <QHideEvent>
#include <QHeaderView>

#include <common/ui_state/workbench_state_manager.h>

MaterialsWorkbench::MaterialsWorkbench(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("materials_tab");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    _table = new MaterialTableWidget(this);
    _table->setObjectName("materials_table");
    _table->horizontalHeader()->setObjectName("materials_header");

    layout->addWidget(_table);

    _manager = new MaterialTableManager(_table, this);
    _manager->populateAll();
}

// void MaterialsWorkbench::hideEvent(QHideEvent* e)
// {
//     // WorkbenchStateManager::instance().onTabDeactivated(this);
//     // QWidget::hideEvent(e);
// }

// bool MaterialsWorkbench::event(QEvent* e)
// {
//     if (e->type() == QEvent::ShowToParent) {
//         zInfo("✅ ShowToParent");

//         // Tab ténylegesen megjelent → restore
//        WorkbenchStateManager::instance().onTabActivated(this);
//     }
//     else
//         if (e->type() == QEvent::HideToParent) {
//         zInfo("✅ HideToParent");

//         // Tab épp eltűnik → még él → save
//        WorkbenchStateManager::instance().onTabDeactivated(this);
//     }
//     return QWidget::event(e);
// }
