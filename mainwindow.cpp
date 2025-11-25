#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"
#include "common/settings/settings_manager.h"
#include "common/utils/qt_event_util.h"

#include "materials/view/material_table_widget.h"
#include "materials/view/material_table_manager.h"
#include "materials/repository/material_repository.h"
#include "materials/registry/material_registry.h"

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
        ui->splitter->restoreState(SettingsManager::instance().mainSplitterState());

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

    // 1) Anyagok betöltése registry-be (CSV)
    loadMaterials();

    // 2) Viewer fül inicializálása és feltöltése
    initMaterialsTab();

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

    // — Aktív tab mentése (opcionális) —
    SettingsManager::instance().setCurrentTabIndex(ui->tabWidget->currentIndex());

    SettingsManager::instance().save();
    event->accept();
}

/**/

void MainWindow::loadMaterials() {
    // Betölti a materials.csv-t → registry.setData(...)
    // Ha a FileNameHelper nincs inicializálva, a repo false-szal tér vissza.
    MaterialRepository::loadFromCSV(MaterialRegistry::instance());
}

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
