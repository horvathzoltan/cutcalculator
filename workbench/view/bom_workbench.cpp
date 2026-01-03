// workbench/view/bom_workbench.cpp
#include "workbench/view/bom_workbench.h"

#include <QHeaderView>
#include <QAction>
#include <QLabel>

//#include "common/settings/settings_manager.h"
#include "common/logger/event_logger.h"
#include "needs/manager/material_requirements_manager.h"
#include "needs/view/material_picker_dialog.h"
#include "needs/view/material_requirements_view.h"

#include "products/view/product_tree_manager.h"
//#include "needs/repository/need_rule_repository.h"

#include "calculation/manager/calculation_mode_detail_manager.h"
//#include "needscalculation/registry/need_calculation_detail_registry.h"
//#include "common/registry/registry_manager.h"

#include "common/utils/geometry_helper.h"
#include "common/utils/qt_event_util.h"

#include "common/layout/layout_default_store.h"
#include "common/snapshot/snapshot_manager.h"


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
    _layout->addWidget(_splitter);

    // Bal panel: terméktípus-fa
    buildLeftPanel();

    // Jobb panel: BOM tabok
    buildRightPanel();

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
         restoreState();
         _restoredOnce = true;
         zEventINFO("🧩 BOMWorkbench state restored (showEvent + queued)");
    });

    _isFullyShown = true;

    if (!this->isWindow()) {
        zInfo("⚠️ BOMWorkbench is NOT a top-level window → closeEvent() will NEVER fire");
    }
}

QToolBar* BOMWorkbench::buildTreeToolbar(QWidget* parent) {
    QToolBar* treeToolbar = new QToolBar("Fa műveletek", parent);
    QAction* addRootAction   = treeToolbar->addAction("➕ Új termékcsoport");
    QAction* addChildAction  = treeToolbar->addAction("➕ Új terméktípus");
    QAction* renameAction    = treeToolbar->addAction("✏️ Átnevezés");
    QAction* removeAction    = treeToolbar->addAction("🗑️ Törlés");

    connect(addRootAction,  &QAction::triggered, _treeManager, &ProductTreeManager::addRootProduct);
    connect(addChildAction, &QAction::triggered, _treeManager, &ProductTreeManager::addChildProduct);
    connect(renameAction,   &QAction::triggered, _treeManager, &ProductTreeManager::renameProduct);
    connect(removeAction,   &QAction::triggered, _treeManager, &ProductTreeManager::removeProduct);

    return treeToolbar;
}

QToolBar* BOMWorkbench::buildMaterialToolbar(QWidget* parent, MaterialRequirementsView* mat_view) {
    QToolBar* matToolbar = new QToolBar("Anyagszükséglet műveletek", parent);
    QAction* addMaterialAction    = matToolbar->addAction("➕ Anyag hozzárendelése");
    QAction* removeMaterialAction = matToolbar->addAction("🗑️ Anyag törlése");

    connect(addMaterialAction, &QAction::triggered, this, [this, mat_view]() {
        MaterialPickerDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            auto res = dlg.result();

            // Registry-be is szúrjuk
            NeedRule rule;
            rule.leftId = _treeManager->currentProductId(); // productId
            rule.rightId = res.material_id;                 // materialId
            NeedRuleRegistry::instance().insert(rule);

            // View frissítés
            MaterialRequirementsView::RequirementRow row;
            row.product_id = _treeManager->currentProductId();
            row.product_name = _treeManager->currentProductName();
            row.product_barcode = _treeManager->currentProductBarcode();
            row.material_id = res.material_id;
            row.material_name = res.material_name;
            row.material_barcode = res.material_barcode;
            row.material_exists = true;

            mat_view->add_requirement(row);
            zEventINFO(QString("➕ Anyag hozzárendelve: %1 → %2")
                           .arg(row.product_name, row.material_name));
        }
    });


    connect(removeMaterialAction, &QAction::triggered, mat_view, [mat_view]() {
        mat_view->remove_selected();
        // Registry törlés a view signal alapján (MaterialRequirementsManager kezeli)
    });

    return matToolbar;
}

QToolBar* BOMWorkbench::buildModesToolbar(QWidget* parent, CalculationModesView* modes_view) {
    QToolBar* modesToolbar = new QToolBar("Számítási mód műveletek", parent);

    // 1) Dokumentum ikon widget
    auto* status = new OverlayIconWidget();
    //status->setFixedSize(40, 40);
    status->setBaseEmoji("📄");

    modesToolbar->addWidget(status);
    modes_view->setStatusWidget(status);

    modes_view->updateOverlayState();   // <<< EZ KELL

    QAction* addModeAction    = modesToolbar->addAction("➕ Új számítási mód");
    QAction* removeModeAction = modesToolbar->addAction("🗑️ Mód törlése");
    QAction* renameModeAction = modesToolbar->addAction("✏️ Átnevezés");

    connect(addModeAction, &QAction::triggered, this, [this, modes_view]() {
        if (!_treeManager)
            return;

        const QUuid productId = _treeManager->currentProductId();
        emit modes_view->request_add_mode(productId);
    });

    connect(removeModeAction, &QAction::triggered, this, [modes_view]() {
        auto modeIdOpt = modes_view->currentModeId();
        if (modeIdOpt)
            emit modes_view->request_remove_mode(*modeIdOpt);
    });

    connect(renameModeAction, &QAction::triggered, this, [modes_view]() {
        auto modeIdOpt = modes_view->currentModeId();
        if (modeIdOpt)
            emit modes_view->request_rename_mode(*modeIdOpt);
    });

    return modesToolbar;
}


QToolBar* BOMWorkbench::buildDetailsToolbar(QWidget* parent, CalculationModeDetailView* detail_view) {
    QToolBar* detailsToolbar = new QToolBar("Formula műveletek", parent);

    QAction* addDetailAction    = detailsToolbar->addAction("➕ Új formula");
    QAction* removeDetailAction = detailsToolbar->addAction("🗑️ Formula törlése");
    QAction* editDetailAction   = detailsToolbar->addAction("✏️ Formula szerkesztése");

    // Új formula hozzáadása
    connect(addDetailAction, &QAction::triggered, this, [this, detail_view]() {
        if (!_modesView)
            return;

        auto modeIdOpt = _modesView->currentModeId();
        if (modeIdOpt)
            emit detail_view->request_add_detail(*modeIdOpt);
    });

    // Formula törlése
    connect(removeDetailAction, &QAction::triggered, this, [detail_view]() {
        auto tables = detail_view->findChildren<QTableWidget*>();
        if (tables.isEmpty())
            return;

        QTableWidget* table = tables.first();
        auto ranges = table->selectedRanges();
        if (ranges.isEmpty())
            return;

        int row = ranges.first().topRow();
        QTableWidgetItem* item = table->item(row, 0);
        if (!item)
            return;

        emit detail_view->request_remove_detail(item->data(Qt::UserRole).toUuid());
    });

    // Formula szerkesztése
    connect(editDetailAction, &QAction::triggered, this, [detail_view]() {
        auto tables = detail_view->findChildren<QTableWidget*>();
        if (tables.isEmpty())
            return;

        QTableWidget* table = tables.first();
        auto ranges = table->selectedRanges();
        if (ranges.isEmpty())
            return;

        int row = ranges.first().topRow();
        QTableWidgetItem* item = table->item(row, 0);
        if (!item)
            return;

        emit detail_view->request_edit_formula(item->data(Qt::UserRole).toUuid());
    });

    return detailsToolbar;
}




void BOMWorkbench::buildLeftPanel() {
    auto* leftWidget = new QWidget(_splitter);
    auto* leftLayout = new QVBoxLayout(leftWidget);

    _treeView = new ProductTreeView(leftWidget);
    _treeManager = new ProductTreeManager(_treeView, this);
    _treeManager->populate();

    leftLayout->addWidget(buildTreeToolbar(leftWidget));
    leftLayout->addWidget(_treeView);
    leftWidget->setLayout(leftLayout);
    _splitter->addWidget(leftWidget);
}


void BOMWorkbench::buildRightPanel() {
    // fő horizontal splitter: bal vertical + jobb details
    _rightMainSplitter = new QSplitter(Qt::Horizontal, _splitter);

    // bal vertical splitter: NeedRules + Modes
    _leftVerticalSplitter = new QSplitter(Qt::Vertical, _rightMainSplitter);

    // NeedRules view + manager
    _matView = new MaterialRequirementsView(_leftVerticalSplitter);
    _matManager = new MaterialRequirementsManager(_matView, this);
    _matToolbar = buildMaterialToolbar(_leftVerticalSplitter, _matView);

    // Modes view + manager
    _modesView = new CalculationModesView(_leftVerticalSplitter);
    _modesManager = new CalculationModesManager(_modesView, this);
    _modesToolbar = buildModesToolbar(_leftVerticalSplitter, _modesView);

    // bal vertical splitter feltöltése két külön containerrel
    auto* matContainer = new QWidget(_leftVerticalSplitter);
    auto* matLayout = new QVBoxLayout(matContainer);
    matLayout->setContentsMargins(0,0,0,0);
    matLayout->addWidget(_matToolbar);
    matLayout->addWidget(_matView);
    matContainer->setLayout(matLayout);

    auto* modesContainer = new QWidget(_leftVerticalSplitter);
    auto* modesLayout = new QVBoxLayout(modesContainer);
    modesLayout->setContentsMargins(0,0,0,0);
    modesLayout->addWidget(_modesToolbar);
    modesLayout->addWidget(_modesView);
    modesContainer->setLayout(modesLayout);

    _leftVerticalSplitter->addWidget(matContainer);
    _leftVerticalSplitter->addWidget(modesContainer);

    // jobb oldal: Details view + manager
    _detailView = new CalculationModeDetailView(_rightMainSplitter);
    _detailManager = new CalculationModeDetailManager(_detailView, this);
    _detailsToolbar = buildDetailsToolbar(_rightMainSplitter, _detailView);

    auto* rightContainer = new QWidget(_rightMainSplitter);
    auto* rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->addWidget(_detailsToolbar);
    rightLayout->addWidget(_detailView);
    rightContainer->setLayout(rightLayout);

    // splitter összeállítás
    _rightMainSplitter->addWidget(_leftVerticalSplitter);
    _rightMainSplitter->addWidget(rightContainer);
    _splitter->addWidget(_rightMainSplitter);

    // wiring: product selection → NeedRules + Modes refresh
    connect(_treeManager, &ProductTreeManager::currentProductChanged,
            this, [this](const QUuid& id, const QString& name, const QString& barcode) {
                if (_matManager)   _matManager->refreshForProduct(id, name, barcode);
                if (_modesManager) _modesManager->refreshForProduct(id, name, barcode);
            });

    // wiring: mode selection → details refresh
    connect(_modesView, &CalculationModesView::selection_changed,
            this, [this](std::optional<QUuid> modeId) {
                if (!modeId) {
                    _detailView->set_details({});
                    return;
                }
                const NeedCalculation *mode =
                    NeedCalculationRegistry::instance().findById(*modeId);
                QString modeName = mode ? mode->name : QString("unknown");
                _detailManager->refreshForCalculation(*modeId, modeName);
            });

    zEventINFO("🧱 BOMWorkbench right panel built (vertical left + details right)");
}


// void BOMWorkbench::restoreState() {
//     if (_leftVerticalSplitter) {
//         QByteArray leftState = SettingsManager::instance().leftVerticalSplitterState();
//         if (!leftState.isEmpty()) _leftVerticalSplitter->restoreState(leftState);
//     }

//     if (_splitter) {
//         QByteArray splitterState = SettingsManager::instance().productTypesSplitterState();
//         if (!splitterState.isEmpty()) _splitter->restoreState(splitterState);
//     }
//     if (_rightMainSplitter) {
//         QByteArray rightState = SettingsManager::instance().mainSplitterState();
//         if (!rightState.isEmpty()) _rightMainSplitter->restoreState(rightState);
//     }
//     // a startupmanagerben betöltésre kerülnek!
//     //NeedRuleRepository::load();
//     if (_treeView && _treeView->header()) {
//         QByteArray headerState = SettingsManager::instance().productTreeHeaderState();
//         if (!headerState.isEmpty()) _treeView->header()->restoreState(headerState);
//     }
//     zEventINFO("BOMWorkbench state restored");
// }

// void BOMWorkbench::saveState() {
//     if (_leftVerticalSplitter)
//         SettingsManager::instance().setLeftVerticalSplitterState(_leftVerticalSplitter->saveState());
//     if (_splitter) SettingsManager::instance().setProductTypesSplitterState(_splitter->saveState());
//     if (_rightMainSplitter) SettingsManager::instance().setMainSplitterState(_rightMainSplitter->saveState());
//     if (_treeView && _treeView->header())
//         SettingsManager::instance().setProductTreeHeaderState(_treeView->header()->saveState());
//     SettingsManager::instance().save();
//     zEventINFO("BOMWorkbench state saved");
// }
void BOMWorkbench::restoreState()
{
    const QString profile =
        SnapshotManager::instance().monitorProfileFor(this);

    zEventINFO(QString("🔍 Loading BOMWorkbench snapshot for profile '%1'")
                   .arg(profile));

    // 1) Monitorprofilhoz kötött snapshot betöltése
    WorkbenchSnapshot snap =
        SnapshotManager::instance().loadWorkbenchSnapshot(this);

    // 2) Bal vertical splitter – snapshot vagy fallback
    if (_leftVerticalSplitter) {
        const QString leftState = !snap.leftVertical.isEmpty()
        ? snap.leftVertical
        : LayoutDefaultStore::instance().leftVerticalSplitterPercent();
        if (!leftState.isEmpty()) {
            GeometryHelper::restoreSplitterState(_leftVerticalSplitter, leftState);
        }
    }

    // 3) Fő splitter (_splitter)
    if (_splitter) {
        const QString splitState = !snap.productTypes.isEmpty()
        ? snap.productTypes
        : LayoutDefaultStore::instance().productTypesSplitterPercent();
        if (!splitState.isEmpty()) {
            GeometryHelper::restoreSplitterState(_splitter, splitState);
        }
    }

    // 4) Jobb main splitter
    if (_rightMainSplitter) {
        const QString rightState = !snap.rightVertical.isEmpty()
        ? snap.rightVertical
        : LayoutDefaultStore::instance().rightVerticalSplitterPercent();
        if (!rightState.isEmpty()) {
            GeometryHelper::restoreSplitterState(_rightMainSplitter, rightState);
        }
    }

    // 5) Tree header
    if (_treeView && _treeView->header()) {
        const QString headerState = !snap.treeHeader.isEmpty()
        ? snap.treeHeader
        : LayoutDefaultStore::instance().productTreeHeaderPercent();
        if (!headerState.isEmpty()) {
            GeometryHelper::restoreHeaderState(_treeView->header(), headerState);
        }
    }

    zEventINFO("BOMWorkbench state restored (percent-based + snapshot-aware)");
}



void BOMWorkbench::saveState()
{
    if (!_isFullyShown) {
        zEventINFO("⏳ Workbench snapshot skipped: Workbench not fully shown yet");
        return;
    }

    WorkbenchSnapshot snap;

    // 1) Bal vertical splitter – fallback + snapshot
    if (_leftVerticalSplitter) {
        const QString leftState = GeometryHelper::saveSplitterState(_leftVerticalSplitter);
        LayoutDefaultStore::instance().setLeftVerticalSplitterPercent(leftState);
        snap.leftVertical = leftState;
    }

    // 2) Fő splitter
    if (_splitter) {
        const QString splitState = GeometryHelper::saveSplitterState(_splitter);
        LayoutDefaultStore::instance().setProductTypesSplitterPercent(splitState);
        snap.productTypes = splitState;
    }

    // 3) Jobb main splitter
    if (_rightMainSplitter) {
        const QString rightState = GeometryHelper::saveSplitterState(_rightMainSplitter);
        LayoutDefaultStore::instance().setRightVerticalSplitterPercent(rightState);
        snap.rightVertical = rightState;
    }

    // 4) Tree header
    if (_treeView && _treeView->header()) {
        const QString headerState = GeometryHelper::saveHeaderState(_treeView->header());
        LayoutDefaultStore::instance().setProductTreeHeaderPercent(headerState);
        snap.treeHeader = headerState;
    }

    // Fallback settings flush – settings.ini-ben mindig naprakész baseline
    LayoutDefaultStore::instance().flush();

    // Monitor-profilos snapshot mentése (per monitor layout)
    SnapshotManager::instance().saveWorkbenchSnapshot(snap, this);

    zEventINFO("BOMWorkbench state saved (percent-based + snapshot-aware)");
}

// void BOMWorkbench::closeEvent(QCloseEvent* e) {
//     saveState();
//     QWidget::closeEvent(e);
// }


