#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"
#include "common/settings/settings_manager.h"
//#include "common/utils/qt_event_util.h"

#include "materials/view/material_table_widget.h"
#include "materials/view/material_table_manager.h"

#include "workbench/view/bom_workbench.h"

#include <QHeaderView>
#include <QTimer>
#include <QToolBar>

#include "common/utils/geometry_helper.h"

#include "common/layout/layout_default_store.h"
#include "common/snapshot/snapshot_manager.h"

#include <common/system/window_stability_monitor.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initEventLogWidget();

    WindowStabilityMonitor::instance().attachTo(this);

    connect(&WindowStabilityMonitor::instance(), &WindowStabilityMonitor::windowStable,
            this, &MainWindow::onWindowStable);


    // 1. Materials tab  inicializálása és feltöltése
    initMaterialsTab();
    // 2. BOM Workbench tab
    initBOMWorkbenchTab();

    zEvent("✅ MainWindow inited");
}

void MainWindow::onWindowStable()
{
    // 1) Snapshot restore – ha van
    const bool restored =
        SnapshotManager::instance().restoreSnapshot_MainWindow(this);

    if (!restored) {
        // 2) fallback percent restore
        const QString geom = LayoutDefaultStore::instance().windowGeometryPercent();
        const QSize savedScreen =
            GeometryHelper::parseScreenSize(LayoutDefaultStore::instance().screenSizeString());
        if (!geom.isEmpty())
            GeometryHelper::restoreWindowGeometry(this, geom, savedScreen);

        const QString split = LayoutDefaultStore::instance().mainSplitterPercent();
        if (!split.isEmpty())
            GeometryHelper::restoreSplitterState(ui->splitter, split);
    }

    // 3) BOMWorkbench és társai
    emit finalPlacementReached();

    // 3/b) Aktív tab visszaállítása — RESTORE ONLY
    int savedTab = SettingsManager::instance().currentTabIndex();
    int tabMax = ui->tabWidget->count();

    if (savedTab >= 0 && savedTab < tabMax) {
        ui->tabWidget->setCurrentIndex(savedTab);
        zInfo(QString("🔄 Active tab restored: %1 / %2").arg(savedTab).arg(tabMax));
    } else {
        zWarning(QString("⚠️ Active tab index invalid → NOT overwriting saved value: %1 / %2")
                     .arg(savedTab).arg(tabMax));
        // ❗ NINCS fallback 0
        // ❗ NINCS visszaírás
    }

    // 4) snapshot mentés stabil geometriáról
    SnapshotManager::instance().saveSnapshot_MainWindow(this);
}

MainWindow::~MainWindow()
{
    delete _logAdapter;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
        SnapshotManager::instance().saveSnapshot_MainWindow(this);
    }
}

void MainWindow::moveEvent(QMoveEvent* e) {
    QMainWindow::moveEvent(e);

    _lastSeenProfile = SnapshotManager::instance().monitorProfileFor(this);

    // if (!_windowRestoredOnce) {
    //     QTimer::singleShot(200, this, &MainWindow::checkFinalPlacement);
    // }

    if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
        SnapshotManager::instance().saveSnapshot_MainWindow(this);
    }
}

void MainWindow::changeEvent(QEvent* e) {
    if (e->type() == QEvent::WindowStateChange) {
        if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
            SnapshotManager::instance().saveSnapshot_MainWindow(this);
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

    BOMWorkbenchSaveState();

    // Aktív tab mentése – ez továbbra is klasszikus setting
    //SettingsManager::instance().setCurrentTabIndex(ui->tabWidget->currentIndex());
    int idx = ui->tabWidget->currentIndex();
    int max = ui->tabWidget->count();

    if (idx >= 0 && idx < max) {
        SettingsManager::instance().setCurrentTabIndex(idx);
        zInfo(QString("💾 Saved active tab index: %1").arg(idx));
    } else {
        zWarning(QString("⚠️ NOT saving tab index → invalid at close time: %1 / %2")
                     .arg(idx).arg(max));
    }

    // Fallback settings flush
    LayoutDefaultStore::instance().flush();
    event->accept();
}

// Ha a BOMWorkbench nem top-level widget, akkor NEM kap closeEvent-et,
// ezért a MainWindow-nak kell meghívnia a saveState()-et.

void MainWindow::BOMWorkbenchSaveState(){
    BOMWorkbench* bom = ui->tabWidget->findChild<BOMWorkbench*>();
    if(!bom) return;
    if(bom->isWindow()) return; // van sajátja majd menti magának ahogy megtanulta

    bom->saveUiState();
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

void MainWindow::initBOMWorkbenchTab()
{
    auto* bomTab = new BOMWorkbench(ui->tabWidget);
    ui->tabWidget->addTab(bomTab, tr("BOM Workbench"));
}
