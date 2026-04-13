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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initEventLogWidget();

    // — Ablak geometria visszaállítása —
    // ablakméret - az esemény időzítve (Qt event queue-ban)
    QTimer::singleShot(0, this, [this]() {
        _initialMonitorProfile = SnapshotManager::instance().monitorProfileFor(this);
        zInfo(QString("🖥️ Initial monitor profile: %1").arg(_initialMonitorProfile));

        // snapshot létezésének ellenőrzése
        const bool hasSnapshot =
            SnapshotManager::instance().restoreSnapshot_MainWindow(this);

        // fallback csak akkor fusson, ha nincs snapshot
        if (!hasSnapshot) {
            // fallback window restore csak akkor fusson, ha nincs snapshot
            if (!hasSnapshot) {
                const QString geom = LayoutDefaultStore::instance().windowGeometryPercent();
                const QSize savedScreen =
                    GeometryHelper::parseScreenSize(LayoutDefaultStore::instance().screenSizeString());
                if (!geom.isEmpty()) {
                    GeometryHelper::restoreWindowGeometry(this, geom, savedScreen);
                }
            }

        }

        // fallback splitter restore csak akkor fusson, ha nincs snapshot
        if (!hasSnapshot) {
            const QString split = LayoutDefaultStore::instance().mainSplitterPercent();
            if (!split.isEmpty()) {
                GeometryHelper::restoreSplitterState(ui->splitter, split);
            }
        }


        // — Aktív tab (opcionális) —
        int savedTab = SettingsManager::instance().currentTabIndex();
        int tabMax = ui->tabWidget->count();
        if (savedTab >= 0 && savedTab < tabMax) {
            ui->tabWidget->setCurrentIndex(savedTab);
            zInfo("✅ Tab index restored (delayed): " + QString::number(savedTab)+" of "+QString::number(tabMax)+" tabs");
        } else{
            zInfo("⚠️ Tab index cannot restored (delayed): " + QString::number(savedTab)+" of "+QString::number(tabMax)+" tabs");
        }

        zInfo("✅ UI Settings loaded (percent-based + snapshot-aware, early restore)");
    });



    // 1) Materials tab  inicializálása és feltöltése
    initMaterialsTab();

    // BOM Workbench tab
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

void MainWindow::showEvent(QShowEvent* e) {
    QMainWindow::showEvent(e);

    static bool logged = false;
    if (!logged) {
        logged = true;

        QScreen* s = this->screen();
        if (s) {
            zInfo(QString("🪟 showEvent: window is now visible on screen %1 (%2x%3, %4dpi)")
                      .arg(s->name())
                      .arg(s->size().width())
                      .arg(s->size().height())
                      .arg(int(std::lround(s->logicalDotsPerInch()))));
        } else {
            zInfo("🪟 showEvent: window is visible, but screen() is null");
        }
    }

    _lastSeenProfile = SnapshotManager::instance().monitorProfileFor(this);
    QTimer::singleShot(200, this, &MainWindow::checkFinalPlacement);
}


void MainWindow::resizeEvent(QResizeEvent* e) {
    QMainWindow::resizeEvent(e);
    if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
        SnapshotManager::instance().saveSnapshot_MainWindow(this);
    }
}

// void MainWindow::moveEvent(QMoveEvent* e) {
//     QMainWindow::moveEvent(e);
//     if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
//         SnapshotManager::instance().saveSnapshot_MainWindow(this);
//     }
// }
void MainWindow::moveEvent(QMoveEvent* e) {
    QMainWindow::moveEvent(e);

    _lastSeenProfile = SnapshotManager::instance().monitorProfileFor(this);

    if (!_windowRestoredOnce) {
        QTimer::singleShot(200, this, &MainWindow::checkFinalPlacement);
    }

    if (_windowRestoredOnce && GeometryHelper::isWindowGeometryReady(this)) {
        SnapshotManager::instance().saveSnapshot_MainWindow(this);
    }
}


void MainWindow::checkFinalPlacement()
{
    if (_windowRestoredOnce) {
        return;
    }

    const QString current = SnapshotManager::instance().monitorProfileFor(this);

    if (current != _initialMonitorProfile) {
        zInfo(QString("🖥️ Monitor profile stabilized: %1 → %2")
                  .arg(_initialMonitorProfile, current));
    } else {
        zInfo(QString("🖥️ Monitor profile stabilized without change (%1)").arg(current));
    }

    const bool restored = SnapshotManager::instance().restoreSnapshot_MainWindow(this);
    _windowRestoredOnce = true;

    if (!restored) {
        zWarning("⚠️ No geometry snapshot for final monitor profile; keeping current geometry");
    }

    // final placement után: splitter restore csak egyszer, snapshot alapján
    const QString split = LayoutDefaultStore::instance().mainSplitterPercent();
    if (!split.isEmpty()) {
        GeometryHelper::restoreSplitterState(ui->splitter, split);
    }

    emit finalPlacementReached();
    SnapshotManager::instance().saveSnapshot_MainWindow(this);


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
    SettingsManager::instance().setCurrentTabIndex(ui->tabWidget->currentIndex());

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

    bom->saveState();
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

// bool MainWindow::event(QEvent* e)
// {
//     // 🎯 Ha ez egy LambdaEvent, akkor futtatjuk a benne levő lambdát
//     if (e->type() == QEvent::User) {
//         auto* lambdaEvent = static_cast<LambdaEvent*>(e);
//         lambdaEvent->execute();
//         return true; // jelezzük, hogy kezeltük
//     }
//     // 🔄 Egyéb események átadása az alapkezelésnek
//     return QMainWindow::event(e); // minden más esemény átadva az alapnak
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

