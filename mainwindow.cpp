#include "mainwindow.h"
#include "common/ui_state/widget_state_manager.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"
#include "common/settings/settings_manager.h"
//#include "common/utils/qt_event_util.h"

#include "materials/view/material_table_widget.h"
#include "materials/view/material_table_manager.h"

#include "workbench/view/bom/bom_workbench.h"
#include "workbench/view/order/order_workbench.h"

#include <QHeaderView>
#include <QTimer>
#include <QToolBar>

#include "common/utils/window_geometry_helper.h"

#include "common/layout/layout_default_store.h"

#include <common/system/window_stability_monitor.h>

#include <common/window_state/window_state_manager.h>


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
    // 3. Order Workbench tab
    initOrderWorkbenchTab();

    zEvent("✅ MainWindow inited");
}

/*
 * MainWindow Restore Pipeline – PATCH 11 (Master Overview)
 * --------------------------------------------------------
 * A MainWindow indulásakor két, jól elkülönülő UI-state réteg állítódik vissza:
 *
 *   1) WindowState (px-alapú, monitorprofil-függő)
 *   2) WidgetState (percent-alapú, monitorfüggetlen)
 *
 * A pipeline sorrendje és felelősségi körei:
 *
 * ---------------------------------------------------------------------------
 * 1) WindowState restore  (WindowStateManager)
 * ---------------------------------------------------------------------------
 *   - top-level geometry (x, y, width, height)
 *   - monitor profile (DPI, screen size)
 *   - px-alapú állapot
 *
 *   Ha sikeres:
 *       restored == true
 *       → a MainWindow pontosan a korábbi helyére kerül
 *
 *   Ha sikertelen:
 *       restored == false
 *       → fallback percent restore (LayoutDefaultStore)
 *         (windowGeometryPercent + mainSplitterPercent)
 *
 *
 * ---------------------------------------------------------------------------
 * 2) Tab restore (csak WindowState success esetén)
 * ---------------------------------------------------------------------------
 *   - A tab index px-alapú WindowState restore után stabil
 *   - Ha restored == true → visszaállítjuk a tab indexet
 *   - Ha restored == false → nem állítjuk vissza (mert hibás lehet)
 *
 *
 * ---------------------------------------------------------------------------
 * 3) finalPlacementReached() jelzés
 * ---------------------------------------------------------------------------
 *   - Ekkor a MainWindow már stabil helyen és méretben van
 *   - A WidgetState restore csak ezután futhat
 *   - A WindowState save NEM itt történik
 *
 *
 * ---------------------------------------------------------------------------
 * 4) WidgetState restore  (WidgetStateManager)
 * ---------------------------------------------------------------------------
 *   - WidgetDiscoveryHelper: releváns widgetek összegyűjtése
 *   - WidgetStateSettings: QVariantMap betöltése
 *   - StateHandlers: extract/restore widget-specifikus állapotok
 *
 *   Mentett elemek:
 *       - QSplitter: percent-alapú arányok
 *       - QHeaderView: oszlopszélességek + sort state
 *       - QTabWidget: currentIndex
 *       - QAbstractScrollArea: scroll pozíciók
 *
 *   A WidgetState percent-alapú → monitorfüggetlen.
 *
 *
 * ---------------------------------------------------------------------------
 * 5) WindowState save (NEM itt)
 * ---------------------------------------------------------------------------
 *   - Csak stabilitás után (resize/move/change eventek)
 *   - Throttle + cooldown védi a túl gyakori mentéstől
 *
 *
 * Összefoglalás:
 *   - WindowState → px-alapú, monitorfüggő, top-level
 *   - WidgetState → percent-alapú, monitorfüggetlen, belső widgetek
 *   - A két réteg egymást kiegészíti, nem keveredik
 *   - A restore pipeline sorrendje szigorú és auditálható
 *
 * A működés NEM változott — kizárólag a dokumentáció és logolás lett egységesítve.
 */


void MainWindow::onWindowStable()
{
    if (_restoreInProgress)
        return;
    _restoreInProgress = true;

    // 1) WindowState restore
    const bool restored =
        WindowStateManager::instance().restoreSnapshot_MainWindow(this);

    _windowRestoredOnce = true;

    if (!restored) {
        // 2) Fallback percent restore
        const QString geom = LayoutDefaultStore::instance().windowGeometryPercent();
        const QSize savedScreen =
            WindowGeometryHelper::parseScreenSize(LayoutDefaultStore::instance().screenSizeString());
        if (!geom.isEmpty())
            WindowGeometryHelper::restoreWindowGeometry(this, geom, savedScreen);

        const QString split = LayoutDefaultStore::instance().mainSplitterPercent();
        if (!split.isEmpty())
            WindowGeometryHelper::restoreSplitterState(ui->splitter, split);
    }

    // 3) Tab restore
    // (itt még nem emitálunk)

    // 3/b) Aktív tab visszaállítása — csak ha WindowState restore sikeres volt
    if (restored) {
        int savedTab = SettingsManager::instance().currentTabIndex();
        int tabMax = ui->tabWidget->count();

        if (savedTab >= 0 && savedTab < tabMax) {
            ui->tabWidget->setCurrentIndex(savedTab);
            zInfo().noquote()
                << QString("🔄 [WidgetState] Active tab restored → index=%1 / %2 (WindowState restore OK)")
                       .arg(savedTab)
                       .arg(tabMax);
        } else {
            zWarning().noquote()
            << QString("⚠️ [WidgetState] Active tab index invalid → skipping restore (%1 / %2)")
                    .arg(savedTab)
                    .arg(tabMax);
        }
    }

    // 4) finalPlacementReached
    // snapshot mentés itt NINCS — a mentés csak stabilitás után történik (resize/move/change eventekben)
    emit finalPlacementReached();

    // 5) WidgetState restore
    WidgetStateManager c("mainwindow_ui");
    c.restoreWidgetState(this);

    _windowRestoredOnce = true;
    _restoreInProgress = false;
}

MainWindow::~MainWindow()
{
    delete _logAdapter;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (_windowRestoredOnce &&
        WindowStabilityMonitor::instance().isFullyStable() &&
        !_restoreInProgress &&
        WindowGeometryHelper::isWindowGeometryReady(this)) {
        WindowStateManager::instance().saveSnapshot_MainWindow(this);
    }

}


void MainWindow::moveEvent(QMoveEvent* e) {
    QMainWindow::moveEvent(e);

    _lastSeenProfile = WindowStateManager::instance().monitorProfileFor(this);

    // if (!_windowRestoredOnce) {
    //     QTimer::singleShot(200, this, &MainWindow::checkFinalPlacement);
    // }

    if (_windowRestoredOnce &&
        WindowStabilityMonitor::instance().isFullyStable() &&
        !_restoreInProgress &&
        WindowGeometryHelper::isWindowGeometryReady(this)) {
        WindowStateManager::instance().saveSnapshot_MainWindow(this);
    }
}

void MainWindow::changeEvent(QEvent* e) {
    if (e->type() == QEvent::WindowStateChange) {
        if (_windowRestoredOnce &&
            WindowStabilityMonitor::instance().isFullyStable() &&
            !_restoreInProgress &&
            WindowGeometryHelper::isWindowGeometryReady(this)) {
            WindowStateManager::instance().saveSnapshot_MainWindow(this);
        }
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Window fallback layout percent-based – UiDefaultStore kezeli a settings.ini-t
    const QString geom = WindowGeometryHelper::saveWindowGeometry(this);
    LayoutDefaultStore::instance().setWindowGeometryPercent(geom);

    const QString scr = this->screen()
                            ? WindowGeometryHelper::serializeScreenSize(this->screen()->size())
                            : QString();
    LayoutDefaultStore::instance().setScreenSizeString(scr);

    // Main splitter fallback
    const QString split = WindowGeometryHelper::saveSplitterState(ui->splitter);
    LayoutDefaultStore::instance().setMainSplitterPercent(split);

    //BOMWorkbenchSaveState();

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

    // WidgetState save – belső widgetek állapota
    {
        WidgetStateManager c("mainwindow_ui");
        c.saveWidgetState(this);
    }

    // Snapshot mentés kilépéskor
    WindowStateManager::instance().saveSnapshot_MainWindow(this);

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

void MainWindow::initOrderWorkbenchTab()
{
    auto* orderTab = new OrderWorkbench(ui->tabWidget);
    ui->tabWidget->addTab(orderTab, tr("Orders"));
}
