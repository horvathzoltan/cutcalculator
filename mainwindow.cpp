#include "mainwindow.h"
//#include "products/view/product_tree_view.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"
#include "common/settings/settings_manager.h"
#include "common/utils/qt_event_util.h"

#include "materials/view/material_table_widget.h"
#include "materials/view/material_table_manager.h"
//#include "materials/repository/material_repository.h"
//#include "materials/registry/material_registry.h"

//#include "products/repository/product_repository.h"
//#include "products/registry/product_registry.h"
//#include "products/view/product_tree_manager.h"

#include "workbench/view/bom_workbench.h"

#include <QHeaderView>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initEventLogWidget();

    // — Ablak geometria visszaállítása —
    // ablakméret - az esemény időzítve (Qt event queue-ban)
    QtEventUtil::post(this, [this]() {
        restoreGeometry(SettingsManager::instance().windowGeometry());

        // — Splitter állapot visszaállítása —
        ui->splitter->restoreState(SettingsManager::instance().mainSplitterState());

        zEvent("✅ UI Settings loaded");
        zInfo("✅ UI Settings loaded");
    });

    // — Aktív tab (opcionális) —
    int savedTab = SettingsManager::instance().currentTabIndex();
    if (savedTab >= 0 && savedTab < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(savedTab);
    }

    //loadMaterials(); // 1) Anyagok betöltése registry-be (CSV)
    initMaterialsTab(); // 2) Viewer fül inicializálása és feltöltése

    //loadProductDefinitions();     // 1) Betöltés a registry-be
    //initProductTypesTab();     // 2) Tab + fa felépítése

    // BOM Workbench fül
    {
        auto* bomTab = new BOMWorkbench(ui->tabWidget);
        ui->tabWidget->addTab(bomTab, tr("BOM Workbench"));
    }

    zEvent("✅ MainWindow inited");
}

MainWindow::~MainWindow()
{
    delete _logAdapter;
    delete ui;
}

void MainWindow::initEventLogWidget() {
    if(_logAdapter) return;

    _logAdapter = new LogViewAdapter(ui->eventLog);

    // EventLogger élő sorokhoz maradhat, de a visszaolvasás a LogManagerből jön
    EventLogger::instance().emitEvent = [this](const QString& line) {
        _logAdapter->appendColoredLineWithTimestamp(line);
    };

    // Utolsó START óta visszaolvasás
    QStringList recent = EventLogger::instance().readSinceLastStart();
    _logAdapter->appendLines(recent);
}

bool MainWindow::event(QEvent* e)
{
    // 🎯 Ha ez egy LambdaEvent, akkor futtatjuk a benne levő lambdát
    if (e->type() == QEvent::User) {
        auto* lambdaEvent = static_cast<LambdaEvent*>(e);
        lambdaEvent->execute();
        return true; // jelezzük, hogy kezeltük
    }
    // 🔄 Egyéb események átadása az alapkezelésnek
    return QMainWindow::event(e); // minden más esemény átadva az alapnak
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // — Ablak geometria mentése —
    SettingsManager::instance().setWindowGeometry(saveGeometry());

    // — Splitter állapot mentése —
    SettingsManager::instance().setMainSplitterState(ui->splitter->saveState());

    // BOMWorkbench állapot mentés (splitter + fa fejlécek)
    if (auto* bom = ui->tabWidget->findChild<BOMWorkbench*>()) {
        bom->saveState();
    }

    if (_productTypesSplitter) {
        SettingsManager::instance().setProductTypesSplitterState(
            _productTypesSplitter->saveState()
            );
    }

    // — Aktív tab mentése (opcionális) —
    SettingsManager::instance().setCurrentTabIndex(ui->tabWidget->currentIndex());

    SettingsManager::instance().save();
    event->accept();
}

/**/

// void MainWindow::loadMaterials() {
//     // Betölti a materials.csv-t → registry.setData(...)
//     // Ha a FileNameHelper nincs inicializálva, a repo false-szal tér vissza.
//     MaterialRepository::loadFromCSV(MaterialRegistry::instance());
// }

void MainWindow::initMaterialsTab() {
    // Keressünk egy tabot, vagy hozzunk létre egyet programból
    // Feltételezzük, hogy a Designerben van egy QTabWidget: ui->tabWidget
    // Hozzunk létre egy új QWidget-et a tabnak:
    QWidget* materialViewerTab = new QWidget(ui->tabWidget);
    materialViewerTab->setObjectName("materialViewerTab");

    // Tegyünk rá egy vertikális layout-ot és a táblát:
    auto* layout = new QVBoxLayout(materialViewerTab);
    _materialsTable = new MaterialTableWidget(materialViewerTab);
    layout->addWidget(_materialsTable);

    // Manager példány
    _materialsManager = new MaterialTableManager(_materialsTable, this);

    // Tegyük be a tabWidget-be
    ui->tabWidget->addTab(materialViewerTab, tr("Materials"));

    // Töltés (registry → table)
    _materialsManager->populateAll();
}

/*products*/

// void MainWindow::loadProductDefinitions() {
//     bool loaded = ProductRepository::loadFromCSV(ProductRegistry::instance());
//     if(loaded){
//         zInfo(QString("📊 ProductRegistry: %1 terméktípus tárolva").arg(ProductRegistry::instance().size()));
//     }
//     else{
//         zInfo("⚠️ ProductRegistry: terméktípusok betöltése sikertelen");
//     }
// }

// void MainWindow::initProductTypesTab() {
//     // Új tab programból
//     QWidget* productTab = new QWidget(ui->tabWidget);
//     productTab->setObjectName("productTypesTab");
//     ui->tabWidget->addTab(productTab, tr("Product Types"));

//     auto* layout = new QVBoxLayout(productTab);
//     _productTypesSplitter = new QSplitter(Qt::Horizontal, productTab);

//     layout->addWidget(_productTypesSplitter);

//     // Bal oldal: fa
//     _productTreeView = new ProductTreeView(_productTypesSplitter);

//     // Oszlopszélességek visszaállítása
//     QByteArray headerState = SettingsManager::instance().productTreeHeaderState();
//     if (!headerState.isEmpty()) {
//         _productTreeView->header()->restoreState(headerState);
//     }

//     _productTypesSplitter->addWidget(_productTreeView);

//     // Jobb oldal: placeholder (később CalculationRule table)
//     auto* rightPlaceholder = new QWidget(_productTypesSplitter);
//     _productTypesSplitter->addWidget(rightPlaceholder);


//     QByteArray splitterState = SettingsManager::instance().productTypesSplitterState();
//     if (!splitterState.isEmpty()) {
//         _productTypesSplitter->restoreState(splitterState);
//     }


//     // Manager: feltölti a fát
//     _productTreeManager = new ProductTreeManager(_productTreeView, this);
//     _productTreeManager->populate();


// /**
//  * Product Types tab inicializálása.
//  *
//  * - Bal oldal: fa nézet (Termékcsoportok és Terméktípusok hierarchiája).
//  * - Jobb oldal: placeholder (később CalculationRule táblázat).
//  * - Toolbar: új kategória (Termékcsoport), új terméktípus, átnevezés, törlés.
//  *
//  * Terminológia:
//  * - Termékcsoport = kategória (ág, félkövér ciánkék).
//  * - Terméktípus   = levél (dőlt fehér).
//  */

//     // Toolbar a productTab fölé
//     QToolBar* productToolbar = new QToolBar("Product Actions", productTab);
//     layout->insertWidget(0, productToolbar);

//     QAction* addCategoryAction   = productToolbar->addAction("➕ Új kategória");
//     QAction* addTypeAction       = productToolbar->addAction("➕ Új terméktípus");
//     QAction* renameAction   = productToolbar->addAction("✏️ Átnevezés");
//     QAction* removeAction   = productToolbar->addAction("🗑️ Törlés");

//     // Összekötés a manager slotjaival
//     connect(addCategoryAction,  &QAction::triggered, _productTreeManager, &ProductTreeManager::addRootProduct);
//     connect(addTypeAction, &QAction::triggered, _productTreeManager, &ProductTreeManager::addChildProduct);
//     connect(renameAction,   &QAction::triggered, _productTreeManager, &ProductTreeManager::renameProduct);
//     connect(removeAction,   &QAction::triggered, _productTreeManager, &ProductTreeManager::removeProduct);

//     // Kontextmenü a QTreeView-hoz
//     _productTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
//     _productTreeView->addAction(addTypeAction);
//     _productTreeView->addAction(renameAction);
//     _productTreeView->addAction(removeAction);

// }
