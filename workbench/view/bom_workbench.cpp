// workbench/view/bom_workbench.cpp
#include "workbench/view/bom_workbench.h"

#include <QHeaderView>
#include <QAction>
#include <QLabel>

#include "common/settings/settings_manager.h"
#include "common/logger/event_logger.h"
#include "needs/view/material_picker_dialog.h"
#include "needs/view/material_requirements_view.h"

#include "products/view/product_tree_manager.h"

BOMWorkbench::BOMWorkbench(QWidget* parent)
    : QWidget(parent)
{
    // Fő layout
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    _toolbar = new QToolBar("BOM Actions", this);
    _layout->addWidget(_toolbar);

    // Splitter
    _splitter = new QSplitter(Qt::Horizontal, this);
    _layout->addWidget(_splitter);

    // Bal panel: terméktípus-fa
    buildLeftPanel();

    // Jobb panel: BOM tabok
    buildRightPanel();

    // Állapot visszaállítása
    restoreState();

    zEventINFO("BOMWorkbench initialized");
}

void BOMWorkbench::buildToolbar() {
    // Azonnal a fa manager slotjaival dolgozunk (CRUD)
    QAction* addRootAction = _toolbar->addAction(QString::fromUtf8("➕ Új termékcsoport"));
    QAction* addChildAction     = _toolbar->addAction(QString::fromUtf8("➕ Új terméktípus"));
    QAction* renameAction      = _toolbar->addAction(QString::fromUtf8("✏️ Átnevezés"));
    QAction* removeAction      = _toolbar->addAction(QString::fromUtf8("🗑️ Törlés"));

    _addMaterialAction = _toolbar->addAction(QString::fromUtf8("➕ Anyag hozzárendelése"));

    // Összekötés akkor történhet meg, amikor a manager már létezik.
    // Itt csak létrehozzuk az action-öket, a connect a buildLeftPanel-ben történik.
    Q_UNUSED(addRootAction)
    Q_UNUSED(addChildAction)
    Q_UNUSED(renameAction)
    Q_UNUSED(removeAction)
    //Q_UNUSED(addMaterialAction)
}

void BOMWorkbench::buildLeftPanel() {
    // Fa nézet
    _treeView = new ProductTreeView(_splitter);
    _splitter->addWidget(_treeView);

    // Fejlécek visszaállítása (ha van mentett állapot)
    const QByteArray headerState = SettingsManager::instance().productTreeHeaderState();
    if (!headerState.isEmpty()) {
        _treeView->header()->restoreState(headerState);
    }

    // Manager
    _treeManager = new ProductTreeManager(_treeView, this);
    _treeManager->populate();

    // Most, hogy van manager, a toolbar action-jeit összekötjük
    buildToolbar();
    auto actions = _toolbar->actions();
    // Sorrend: [0] addCategory, [1] addType, [2] rename, [3] remove
    if (actions.size() >= 5) {
        QObject::connect(actions[0], &QAction::triggered, _treeManager, &ProductTreeManager::addRootProduct);
        QObject::connect(actions[1], &QAction::triggered, _treeManager, &ProductTreeManager::addChildProduct);
        QObject::connect(actions[2], &QAction::triggered, _treeManager, &ProductTreeManager::renameProduct);
        QObject::connect(actions[3], &QAction::triggered, _treeManager, &ProductTreeManager::removeProduct);



    }

    // Kontext menü közvetlenül a fa nézethez
    _treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    for (int i = 1; i < actions.size(); ++i) { // addType, rename, remove
        _treeView->addAction(actions[i]);
    }


    // >>> Itt jön a connect a jobb oldali view-hoz <<<
    if (auto* mat_view = qobject_cast<MaterialRequirementsView*>(_materialsTab)) {
        connect(_treeManager, &ProductTreeManager::currentProductChanged,
                mat_view, [mat_view](const QUuid& id,
                           const QString& name,
                           const QString& barcode) {
                    mat_view->set_current_product(id, name, barcode);
                    // TODO: később: requirements betöltése repo-ból és set_requirements(...)
                });
    }
}

void BOMWorkbench::buildRightPanel() {
    _rightTabs = new QTabWidget(_splitter);
    _splitter->addWidget(_rightTabs);

    // Anyagszükséglet tab – tényleges view
    auto* mat_view = new MaterialRequirementsView(_rightTabs);
    _materialsTab = mat_view;
    _rightTabs->addTab(_materialsTab, QString::fromUtf8("Anyagszükséglet"));

    // Számítási módok tab – placeholder
    _calcModesTab = new QWidget(_rightTabs);
    auto* calcLayout = new QVBoxLayout(_calcModesTab);
    calcLayout->addWidget(new QLabel("Számítási módok (placeholder)\nItt később a CalculationModesView él.", _calcModesTab));
    _rightTabs->addTab(_calcModesTab, QString::fromUtf8("Számítási módok"));

    // >>> Itt jön a connect, mert ekkor már létezik _materialsTab <<<
    if (_addMaterialAction && mat_view) {
        connect(_addMaterialAction, &QAction::triggered, this, [this, mat_view]() {
            MaterialPickerDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {
                auto res = dlg.result();

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
    }
}

void BOMWorkbench::restoreState() {
    // Splitter állapot
    QByteArray splitterState = SettingsManager::instance().productTypesSplitterState();
    if (!splitterState.isEmpty()) {
        _splitter->restoreState(splitterState);
    }

    // Fa fejlécek
    QByteArray headerState = SettingsManager::instance().productTreeHeaderState();
    if (!headerState.isEmpty() && _treeView && _treeView->header()) {
        _treeView->header()->restoreState(headerState);
    }

    zEventINFO("BOMWorkbench state restored");
}

void BOMWorkbench::saveState() {
    // Splitter állapot
    if (_splitter) {
        SettingsManager::instance().setProductTypesSplitterState(_splitter->saveState());
    }

    // Fa fejlécek
    if (_treeView && _treeView->header()) {
        SettingsManager::instance().setProductTreeHeaderState(_treeView->header()->saveState());
    }

    SettingsManager::instance().save();
    zEventINFO("BOMWorkbench state saved");
}
