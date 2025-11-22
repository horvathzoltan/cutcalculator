#include "lifecycle_manager.h"
#include "signal_helper.h"
#include "common/logger/log_manager.h"
#include <QEvent>
#include "common/utils/filename_helper.h"

LifecycleManager& LifecycleManager::instance() {
    static LifecycleManager inst;
    return inst;
}

LifecycleManager::LifecycleManager(QObject* parent)
    : QObject(parent)
{
    observer_ = new WindowObserver(this);
}

void LifecycleManager::setPhase_1(const char* argv0, const QString& appName) {
    // Bináris path beállítása
    FileNameHelper::setBinaryPath(argv0);
    const auto binDir = FileNameHelper::instance().binaryPath();

    // Csatornák init
    LogManager::instance().initChannels(binDir, {LogManager::Channel::Events, LogManager::Channel::Errors});

    // Startup log
    initStartup(appName);
}

void LifecycleManager::setPhase_2(const QString& dataPath) {
    // Mozgatás data könyvtárba (ha van)
    if (!dataPath.isEmpty()) {
        LogManager::instance().moveToFolder(dataPath);
    }
}

void LifecycleManager::setPhase_3(QCoreApplication* app) {
    LifecycleManager::instance().attach(app);
    // Bufferelés bekapcsolása
    LogManager::instance().enableBuffering(true);
    LogManager::instance().setFlushInterval(LogManager::Channel::Events, 2000);
    LogManager::instance().setFlushInterval(LogManager::Channel::Errors, 1000);
}

void LifecycleManager::setPhase_4(QMainWindow* window) {
    if (!window || !observer_) return;
    observer_->setWindow(window);
}

void LifecycleManager::setPhase_4_1() {
    LifecycleManager::instance().markPhase(LifecycleManager::Phase::MainWindowOpen);
}

void LifecycleManager::setPhase_4_2() {
    LifecycleManager::instance().markPhase(LifecycleManager::Phase::MainWindowClose);
}

void LifecycleManager::setPhase_5(int exitCode) {
    _exitCode = exitCode;
}

void LifecycleManager::initStartup(const QString& appName) {
    if (startupMarked_) return;
    startupMarked_ = true;
    _appName = appName;
    markPhase(Phase::Startup);
}

void LifecycleManager::attach(QCoreApplication* app) {
    if (!app) return;
    app_ = app;

    // Application phase (első attach-kor)
    if (!applicationMarked_) {
        applicationMarked_ = true;
        markPhase(Phase::Application);
    }

    // Shutdown automatikus jelzés
    connect(app, &QCoreApplication::aboutToQuit, this, [this]() {
        if (!shutdownMarked_) {
            shutdownMarked_ = true;
            // exit code nem ismert itt; STOP sor előtte/utána is írható
            markPhase(Phase::Shutdown);
        }
    });
}

void LifecycleManager::onAbort(int signal) {
    _exitCode = 128 + signal;
    _signalCode = signal;
    markPhase(Phase::Abort);
}

void LifecycleManager::markPhase(Phase phase) {
    switch (phase) {
    case Phase::Startup:
        LogManager::instance().writeAll("🟢 START application: " + _appName);
        break;
    case Phase::Application:
        LogManager::instance().writeAll("✅ QApplication létrejött");
        break;
    case Phase::MainWindowOpen:
        LogManager::instance().writeAll("🪟 MainWindow megnyílt");
        break;
    case Phase::MainWindowClose:
        LogManager::instance().writeAll("🪟 MainWindow bezárult");
        break;
    case Phase::Shutdown:
        LogManager::instance().writeAll("🛑 STOP application (aboutToQuit). exit code: " + QString::number(_exitCode));
        LogManager::instance().flushAll();
        break;
    case Phase::Abort:
        QString sigName = SignalHelper::signalName(_signalCode);

        LogManager::instance().writeAll("💥 ABORT application ("+sigName+", exit code:"+ QString::number(_exitCode)+")");
        LogManager::instance().flushAll();
        break;
    }
}

