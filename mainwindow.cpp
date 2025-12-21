#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"
#include "common/settings/settings_manager.h"
#include "common/utils/qt_event_util.h"

#include "materials/view/material_table_widget.h"
#include "materials/view/material_table_manager.h"

#include "workbench/view/bom_workbench.h"

#include <QHeaderView>
#include <QToolBar>

#include "common/utils/geometry_helper.h"

#include "common/layout/layout_default_store.h"
#include "common/snapshot/snapshot_manager.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initEventLogWidget();

    // — Ablak geometria visszaállítása —
    // ablakméret - az esemény időzítve (Qt event queue-ban)
    QtEventUtil::post(this, [this]() {
        // 1) Window restore snapshotból (per monitor profil)
        bool restoredFromSnapshot = SnapshotManager::instance().restoreWindowSnapshot(this);

        // 2) Ha nincs snapshot, akkor fallback a UiDefaultStore-ból (settings.ini)
        if (!restoredFromSnapshot) {
            const QString geom = LayoutDefaultStore::instance().windowGeometryPercent();
            const QSize savedScreen = GeometryHelper::parseScreenSize(
                LayoutDefaultStore::instance().screenSizeString());
            if (!geom.isEmpty()) {
                GeometryHelper::restoreWindowGeometry(this, geom, savedScreen);
            }
        }

        // ✅ Itt állítjuk be
        _windowRestoredOnce = true;

        // 3) MainWindow splitter állapot – fallback percent alapú
        const QString split = LayoutDefaultStore::instance().mainSplitterPercent();
        if (!split.isEmpty()) {
            GeometryHelper::restoreSplitterState(ui->splitter, split);
        }

        zEventINFO("✅ UI Settings loaded (percent-based + snapshot-aware)");
    });


    // — Aktív tab (opcionális) —
    int savedTab = SettingsManager::instance().currentTabIndex();
    if (savedTab >= 0 && savedTab < ui->tabWidget->count()) {
        ui->tabWidget->setCurrentIndex(savedTab);
    }

    //loadMaterials(); // 1) Anyagok betöltése registry-be (CSV)
    initMaterialsTab(); // 2) Viewer fül inicializálása és feltöltése

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

void MainWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
        SnapshotManager::instance().saveWindowSnapshot(this);
    }
}

void MainWindow::moveEvent(QMoveEvent* e) {
    QMainWindow::moveEvent(e);
    if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
        SnapshotManager::instance().saveWindowSnapshot(this);
    }
}

void MainWindow::changeEvent(QEvent* e) {
    if (e->type() == QEvent::WindowStateChange) {
        if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
            SnapshotManager::instance().saveWindowSnapshot(this);
        }
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Window fallback layout percent-based – UiDefaultStore kezeli a settings.ini-t
    const QString geom = GeometryHelper::saveWindowGeometry(this);
    LayoutDefaultStore::instance().setWindowGeometryPercent(geom);

    const QString scr = this->screen()
                            ? GeometryHelper::serializeScreenSize(this->screen()->size())
                            : QString();
    LayoutDefaultStore::instance().setScreenSizeString(scr);

    // Main splitter fallback
    const QString split = GeometryHelper::saveSplitterState(ui->splitter);
    LayoutDefaultStore::instance().setMainSplitterPercent(split);

    // BOMWorkbench állapot mentés (ez belül maga is hívja a SnapshotManager-t + UiDefaultStore-t)
    // ezt a BOMWorkbench closeEvent-je kezeli

    // if (auto* bom = ui->tabWidget->findChild<BOMWorkbench*>()) {
    //     bom->saveState();
    // }

    // Aktív tab mentése – ez továbbra is klasszikus setting
    SettingsManager::instance().setCurrentTabIndex(ui->tabWidget->currentIndex());

    // Fallback settings flush
    LayoutDefaultStore::instance().flush();

    event->accept();
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

