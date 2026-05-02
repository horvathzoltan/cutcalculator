// workbench/view/bom_workbench.cpp
#include "bom_workbench.h"

#include <QHeaderView>
#include <QAction>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>

#include "common/logger/event_logger.h"
#include "needs/manager/material_requirements_manager.h"
#include "needs/view/material_requirements_view.h"
#include "products/view/product_tree_manager.h"
#include "calculation/manager/calculation_mode_detail_manager.h"
#include "common/utils/qt_event_util.h"
#include "products/view/product_tree_panel.h"

#include <common/ui_state/workbench_state_manager.h>

BOMWorkbench::BOMWorkbench(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("bom_workbench");
    // Fő layout
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    /*
    _toolbar = new QToolBar("BOM Actions", this);
    _layout->addWidget(_toolbar);
*/
    // Splitter
    _splitter = new QSplitter(Qt::Horizontal, this);
    _splitter->setObjectName("bom_main_splitter");

    _layout->addWidget(_splitter);

    // Bal panel: terméktípus-fa
    buildLeftPanel();

    // Jobb panel: BOM tabok
    buildRightPanel();

    zEventINFO("BOMWorkbench initialized");
}


void BOMWorkbench::buildLeftPanel() {
    auto* leftWidget = new QWidget(_splitter);
    leftWidget->setObjectName("bom_left_panel");
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setObjectName("bom_left_panel_layout");

    _treeView = new ProductTreeView(leftWidget);
    _treeView->setObjectName("bom_product_tree");
    _treeView->header()->setObjectName("bom_tree_header");


    _treeManager = new ProductTreeManager(_treeView, this);
    _treePresenter = new ProductTreePresenter(_treeView, _treeManager, this);//_treePanel
    _treeManager->populate();

    auto* treeToolbar = _treePresenter->buildToolbar(leftWidget);
    treeToolbar->setObjectName("bom_product_tree_toolbar");
    leftLayout->addWidget(treeToolbar);

    leftLayout->addWidget(_treeView);
    leftWidget->setLayout(leftLayout);
    _splitter->addWidget(leftWidget);
}

void BOMWorkbench::buildRightPanel() {
    // fő horizontal splitter: bal vertical + jobb details
    _rightMainSplitter = new QSplitter(Qt::Horizontal, _splitter);
    _rightMainSplitter->setObjectName("bom_right_main_splitter");

    // bal vertical splitter: NeedRules + Modes
    _leftVerticalSplitter = new QSplitter(Qt::Vertical, _rightMainSplitter);
    _leftVerticalSplitter->setObjectName("bom_left_vertical_splitter");

    // NeedRules view + manager
    _matView = new MaterialRequirementsView(_leftVerticalSplitter);
    _matView->setObjectName("bom_mat_view");
    _matView->table()->setObjectName("bom_mat_table");
    _matView->table()->horizontalHeader()->setObjectName("bom_mat_header");


    _matManager = new MaterialRequirementsManager(_matView, this);
    _matPresenter = new MaterialRequirementsPresenter(_matView, _matManager, _treeManager, this);
    _matToolbar = buildMaterialToolbar(_leftVerticalSplitter, _matView);

    // Modes view + manager
    _modesView = new CalculationModesView(_leftVerticalSplitter);
    _modesView->setObjectName("bom_modes_view");
    _modesView->table()->setObjectName("bom_modes_table");
    _modesView->table()->horizontalHeader()->setObjectName("bom_modes_header");


    _modesManager = new CalculationModesManager(_modesView, this);
    _modesPresenter = new CalculationModesPresenter(_modesView, _modesManager, _treeManager, this);
    _modesToolbar = buildModesToolbar(_leftVerticalSplitter, _modesView);

    _modesView->markReady();

    // bal vertical splitter feltöltése két külön containerrel
    auto* matContainer = new QWidget(_leftVerticalSplitter);
    matContainer->setObjectName("bom_mat_container");

    auto* matLayout = new QVBoxLayout(matContainer);
    matLayout->setContentsMargins(0,0,0,0);
    matLayout->addWidget(_matToolbar);
    matLayout->addWidget(_matView);
    matContainer->setLayout(matLayout);

    auto* modesContainer = new QWidget(_leftVerticalSplitter);
    auto* modesLayout = new QVBoxLayout(modesContainer);
    modesContainer->setObjectName("bom_modes_container");

    modesLayout->setContentsMargins(0,0,0,0);
    modesLayout->addWidget(_modesToolbar);
    modesLayout->addWidget(_modesView);
    modesContainer->setLayout(modesLayout);

    _leftVerticalSplitter->addWidget(matContainer);
    _leftVerticalSplitter->addWidget(modesContainer);

    // jobb oldal: Details view + manager
    _detailView = new CalculationModeDetailView(_rightMainSplitter);
    _detailView->setObjectName("bom_detail_view");
    _detailView->table()->setObjectName("bom_detail_table");
    _detailView->table()->horizontalHeader()->setObjectName("bom_detail_header");


    _detailManager = new CalculationModeDetailManager(_detailView, this);
    _detailPresenter = new CalculationModeDetailPresenter(_detailView, _detailManager, this);
    _detailsToolbar = buildDetailsToolbar(_rightMainSplitter, _detailView);
    _detailView->set_details({});
    _detailPresenter->refreshOverlayOnly();

    auto* rightContainer = new QWidget(_rightMainSplitter);
    rightContainer->setObjectName("bom_right_panel");

    auto* rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->addWidget(_detailsToolbar);
    rightLayout->addWidget(_detailView);
    rightContainer->setLayout(rightLayout);

    // splitter összeállítás
    _rightMainSplitter->addWidget(_leftVerticalSplitter);
    _rightMainSplitter->addWidget(rightContainer);
    _splitter->addWidget(_rightMainSplitter);


    QObject::connect(_modesPresenter, SIGNAL(modeSelected(std::optional<QUuid>)),
                     _detailPresenter, SLOT(onModeSelected(std::optional<QUuid>)));

    connect(_modesPresenter, &CalculationModesPresenter::modeActivatedForDetails,
            _detailPresenter, &CalculationModeDetailPresenter::onModeSelected);

    connect(_treeManager, &ProductTreeManager::currentProductChanged,
            this, [this]() {
                if (_matPresenter) _matPresenter->refreshOverlayOnly();
                if (_modesPresenter) _modesPresenter->refreshOverlayOnly();
                if (_detailView) _detailView->set_details({});
                if (_detailPresenter) _detailPresenter->refreshOverlayOnly();
            });   

    zEventINFO("🧱 BOMWorkbench right panel built (vertical left + details right)");
}


QToolBar* BOMWorkbench::buildMaterialToolbar(QWidget* parent, MaterialRequirementsView* view)
{
    Q_UNUSED(view);
    auto* tb = _matPresenter->buildToolbar(parent);
    tb->setObjectName("bom_mat_toolbar");
    return tb;
}

QToolBar* BOMWorkbench::buildModesToolbar(QWidget* parent, CalculationModesView* view)
{
    Q_UNUSED(view);
    auto* tb = _modesPresenter->buildToolbar(parent);
    tb->setObjectName("bom_modes_toolbar");
    return tb;
}

QToolBar* BOMWorkbench::buildDetailsToolbar(QWidget* parent, CalculationModeDetailView* view)
{
    Q_UNUSED(view);
    auto* tb = _detailPresenter->buildToolbar(parent);
    tb->setObjectName("bom_details_toolbar");
    return tb;

}

// void BOMWorkbench::hideEvent(QHideEvent* e)
// {
//     // WorkbenchStateManager::instance().onTabDeactivated(this);
//     // QWidget::hideEvent(e);
// }

