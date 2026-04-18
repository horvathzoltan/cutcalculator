// workbench/view/bom_workbench.cpp
#include "workbench/view/bom_workbench.h"

#include <QHeaderView>
#include <QAction>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>

#include "common/logger/event_logger.h"

#include "common/ui_state/state_collector.h"
#include "needs/manager/material_requirements_manager.h"
#include "needs/view/material_requirements_view.h"

#include "products/view/product_tree_manager.h"

#include "calculation/manager/calculation_mode_detail_manager.h"

#include "common/utils/geometry_helper.h"
#include "common/utils/qt_event_util.h"

//#include "common/layout/layout_default_store.h"
//#include "common/snapshot/snapshot_manager.h"
#include "products/view/product_tree_panel.h"


BOMWorkbench::BOMWorkbench(QWidget* parent)
    : QWidget(parent)
{
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

    if (auto* mw = this->window()) {
        connect(mw, SIGNAL(finalPlacementReached()),
                this, SLOT(onFinalPlacementReached()));
    }

    zEventINFO("BOMWorkbench initialized");
}

// bool BOMWorkbench::event(QEvent* e)
// {
//     //🎯 Ha ez egy LambdaEvent, akkor futtatjuk a benne levő lambdát
//     if (e->type() == QEvent::User) {
//         auto* le = static_cast<LambdaEvent*>(e);
//         le->execute();
//         return true; // jelezzük, hogy kezeltük
//     }
//     // 🔄 Egyéb események átadása az alapkezelésnek
//     return QWidget::event(e); // minden más esemény átadva az alapnak
// }

void BOMWorkbench::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (_restoredOnce) return;

    // Biztosítsuk, hogy a layout már kiosztotta a méreteket
    QTimer::singleShot(0, this, [this]() {
        _restoredOnce = true;
        _isFullyShown = true;
        zEventINFO("🧩 BOMWorkbench fully shown, waiting for final placement");
        tryRestore();
    });

    if (!this->isWindow()) {
        zInfo("⚠️ BOMWorkbench is NOT a top-level window → closeEvent() will NEVER fire");
    }
}


void BOMWorkbench::buildLeftPanel() {
    auto* leftWidget = new QWidget(_splitter);
    leftWidget->setObjectName("bom_left_panel");
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setObjectName("bom_left_panel_layout");

    _treeView = new ProductTreeView(leftWidget);
    _treeView->setObjectName("bom_product_tree");

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

    _matManager = new MaterialRequirementsManager(_matView, this);
    _matPresenter = new MaterialRequirementsPresenter(_matView, _matManager, _treeManager, this);
    _matToolbar = buildMaterialToolbar(_leftVerticalSplitter, _matView);

    // Modes view + manager
    _modesView = new CalculationModesView(_leftVerticalSplitter);
    _modesView->setObjectName("bom_modes_view");

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


// void BOMWorkbench::restoreState()
// {
//     if (!_isFullyShown || !_canRestore) {
//         zEventINFO("⏳ Workbench restore skipped: not fully shown or final placement not reached");
//         return;
//     }

//     const QString profile =
//         SnapshotManager::instance().monitorProfileFor(this);

//     zEventINFO(QString("🔍 Loading BOMWorkbench snapshot for profile '%1'")
//                    .arg(profile));

//     // 1) Monitorprofilhoz kötött snapshot betöltése
//     WorkbenchSnapshot snap =
//         SnapshotManager::instance().restoreSnapshot_BOMWorkbench("bom_workbench");


//     // 2) Bal vertical splitter – snapshot vagy fallback
//     if (_leftVerticalSplitter) {
//         const QString leftState = !snap.leftVertical.isEmpty()
//         ? snap.leftVertical
//         : LayoutDefaultStore::instance().leftVerticalSplitterPercent();
//         if (!leftState.isEmpty()) {
//             GeometryHelper::restoreSplitterState(_leftVerticalSplitter, leftState);
//         }
//     }

//     // 3) Fő splitter (_splitter)
//     if (_splitter) {
//         const QString splitState = !snap.productTypes.isEmpty()
//         ? snap.productTypes
//         : LayoutDefaultStore::instance().productTypesSplitterPercent();
//         if (!splitState.isEmpty()) {
//             GeometryHelper::restoreSplitterState(_splitter, splitState);
//         }
//     }

//     // 4) Jobb main splitter
//     if (_rightMainSplitter) {
//         const QString rightState = !snap.rightVertical.isEmpty()
//         ? snap.rightVertical
//         : LayoutDefaultStore::instance().rightVerticalSplitterPercent();
//         if (!rightState.isEmpty()) {
//             GeometryHelper::restoreSplitterState(_rightMainSplitter, rightState);
//         }
//     }

//     // 5) Tree header
//     if (_treeView && _treeView->header()) {
//         const QString headerState = !snap.treeHeader.isEmpty()
//         ? snap.treeHeader
//         : LayoutDefaultStore::instance().productTreeHeaderPercent();
//         if (!headerState.isEmpty()) {
//             GeometryHelper::restoreHeaderState(_treeView->header(), headerState);
//         }
//     }

//     if (!snap.needRulesHeader.isEmpty()) {
//         GeometryHelper::restoreHeaderState(
//             _matView->table()->horizontalHeader(),
//             snap.needRulesHeader);
//     }

//     if (!snap.modesHeader.isEmpty()) {
//         GeometryHelper::restoreHeaderState(
//             _modesView->table()->horizontalHeader(),
//             snap.modesHeader);
//     }

//     if (!snap.detailsHeader.isEmpty()) {
//         GeometryHelper::restoreHeaderState(
//             _detailView->table()->horizontalHeader(),
//             snap.detailsHeader);
//     }

//     zEventINFO("BOMWorkbench state restored (percent-based + snapshot-aware)");
// }



// void BOMWorkbench::saveState()
// {
//     // 0) Láthatóság + final placement guard
//     if (!_isFullyShown) {
//         zEventINFO("⏳ Workbench snapshot skipped: Workbench not fully shown yet");
//         return;
//     }

//     if (!_canSaveSnapshots) {
//         zEventINFO("⏳ Workbench snapshot skipped: MainWindow not in final placement yet");
//         return;
//     }

//     // 1) Workbench geometry readiness guard – ne mentsünk instabil layoutot
//     if (!GeometryHelper::isWindowGeometryReady(this)) {
//         zEventINFO("⏳ Workbench snapshot skipped: geometry not ready");
//         return;
//     }

//     // 2) Splitter readiness guard – ne mentsünk 0px-es / félig kiosztott splittert
//     auto splitterReady = [](QSplitter* s) {
//         if (!s) return false;
//         const QList<int> sizes = s->sizes();
//         if (sizes.isEmpty()) return false;
//         int sum = 0;
//         for (int v : sizes) sum += v;
//         return sum > 50;
//     };

//     if (!splitterReady(_leftVerticalSplitter) ||
//         !splitterReady(_splitter) ||
//         !splitterReady(_rightMainSplitter)) {

//         zEventINFO("⏳ Workbench snapshot skipped: splitters not ready");
//         return;
//     }

//     WorkbenchSnapshot snap;

//     // 3) Bal vertical splitter – fallback + snapshot
//     if (_leftVerticalSplitter) {
//         const QString leftState = GeometryHelper::saveSplitterState(_leftVerticalSplitter);
//         if (!leftState.isEmpty()) {
//             LayoutDefaultStore::instance().setLeftVerticalSplitterPercent(leftState);
//             snap.leftVertical = leftState;
//         }
//     }

//     // 4) Fő splitter
//     if (_splitter) {
//         const QString splitState = GeometryHelper::saveSplitterState(_splitter);
//         if (!splitState.isEmpty()) {
//             LayoutDefaultStore::instance().setProductTypesSplitterPercent(splitState);
//             snap.productTypes = splitState;
//         }
//     }

//     // 5) Jobb main splitter
//     if (_rightMainSplitter) {
//         const QString rightState = GeometryHelper::saveSplitterState(_rightMainSplitter);
//         if (!rightState.isEmpty()) {
//             LayoutDefaultStore::instance().setRightVerticalSplitterPercent(rightState);
//             snap.rightVertical = rightState;
//         }
//     }

//     // 6) Tree header
//     if (_treeView && _treeView->header()) {
//         const QString headerState = GeometryHelper::saveHeaderState(_treeView->header());
//         if (!headerState.isEmpty()) {
//             LayoutDefaultStore::instance().setProductTreeHeaderPercent(headerState);
//             snap.treeHeader = headerState;
//         }
//     }

//     // 7) NeedRules / Modes / Details headerek
//     if (_matView && _matView->table()) {
//         const QString needState =
//             GeometryHelper::saveHeaderState(_matView->table()->horizontalHeader());
//         if (!needState.isEmpty()) {
//             snap.needRulesHeader = needState;
//         }
//     }

//     if (_modesView && _modesView->table()) {
//         const QString modesState =
//             GeometryHelper::saveHeaderState(_modesView->table()->horizontalHeader());
//         if (!modesState.isEmpty()) {
//             snap.modesHeader = modesState;
//         }
//     }

//     if (_detailView && _detailView->table()) {
//         const QString detailsState =
//             GeometryHelper::saveHeaderState(_detailView->table()->horizontalHeader());
//         if (!detailsState.isEmpty()) {
//             snap.detailsHeader = detailsState;
//         }
//     }

//     // 8) Fallback settings flush – settings.ini-ben mindig naprakész baseline
//     LayoutDefaultStore::instance().flush();

//     // 9) Monitor-profilos snapshot mentése (per monitor layout)
//     SnapshotManager::instance().saveSnapshot_BOMWorkbench(snap, "bom_workbench");

//     zEventINFO("BOMWorkbench state saved (percent-based + snapshot-aware, guarded)");
// }


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
    return _modesPresenter->buildToolbar(parent);
}

QToolBar* BOMWorkbench::buildDetailsToolbar(QWidget* parent, CalculationModeDetailView* view)
{
    Q_UNUSED(view);
    return _detailPresenter->buildToolbar(parent);
}

void BOMWorkbench::tryRestore()
{
    if (!_isFullyShown || !_canRestore)
        return;

    restoreUiState();
}

void BOMWorkbench::restoreUiState()
{
    UIStateCollector c("bom_workbench");
    c.restore(this);
}

void BOMWorkbench::saveUiState()
{
    UIStateCollector c("bom_workbench");
    c.collect(this);
}

