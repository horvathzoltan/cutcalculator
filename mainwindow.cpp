#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/logger/event_logger.h"
#include "ui/adapters/log_view_adapter.h"
#include "common/settings/settings_manager.h"
#include "common/utils/qt_event_util.h"

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
