#include "common/logger/log_manager.h"
#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

#include "common/system/signal_helper.h"
#include "common/logger/event_logger.h"
#include "common/system/sysinfo_helper.h"
#include "common/settings/settings_manager.h"
#include "test/test_manager.h"
#include "common/utils/filename_helper.h"
#include "common/startup/startup_manager.h"
#include "common/system/lifecycle_manager.h"
#include "common/startup/startup_status_manager.h"
#include "common/registry/manager/registry_manager.h"
#include "common/utils/window_geometry_helper.h"

#include "common/system/verbose_class_initializer.h"
#include "common/registry/feature/registry_catalog.h"

#include <expression/function_registry.h>

#include <common/window_state/window_state_manager.h>
//extern void registerAllVerbose();

int main(int argc, char *argv[])
{
    Logger::setBreakOnError(false);
    Logger::setVerbose(false);
#ifdef QT_DEBUG
    Logger::setForceGuiInDebug(true);
#endif
    registerAllVerbose(); // 🐧 Kowalski aktiválva

    QString sysInfo = SysInfoHelper::instance().sysInfo();
    zInfo() << sysInfo;
    zEvent(sysInfo);

    // induláskor - SIGKILL esetén NEM fut le.
    SignalHelper::setCleanupHandler([](int sig){
        LifecycleManager::instance().onAbort(sig);
        if (auto* win = LifecycleManager::instance().mainWindow()) {
            if (WindowGeometryHelper::isWindowGeometryReady(win)) {
                WindowStateManager::instance().saveSnapshot_MainWindow(win);
            }
        }
    });


    SignalHelper::setShutDownSignal(SignalHelper::SIGINT_); // shut down on ctrl-c
    SignalHelper::setShutDownSignal(SignalHelper::SIGTERM_); // shut down on killall

    // 1. Bináris path beállítása
    LifecycleManager::instance().setPhase_1(argv[0], SysInfoHelper::instance().target());

    QCoreApplication::setApplicationName(SysInfoHelper::instance().target());
    QCoreApplication::setApplicationVersion(Buildnumber::value);
    QCoreApplication::setOrganizationName("horvathzoltan");
    QCoreApplication::setOrganizationDomain("https://github.com/horvathzoltan");

    SettingsManager::instance().detectTestMode(argc, argv);
    auto drp = SettingsManager::instance().dataRootPath();
    FileNameHelper::instance().setDataRootPath(drp);

    // Settings betöltése után mozgatás a data könyvtárba
    // Settings után data mappába mozgatás
    const auto dataDir = FileNameHelper::instance().pathFor(FileKind::LogDir, FileAccess::Write);
    LifecycleManager::instance().setPhase_2(dataDir);

    // 🔵 1) Registryk explicit inicializálása
    RegistryCatalog::initializeAll();//initializeAllRegistries();

    // auto& R = FunctionRegistry::instance();
    FunctionRegistry::instance(); // ha a side-effect kell

    // --test eventlogger
    if (SettingsManager::instance().isTestMode()) {
        zInfo("****************************");
        zInfo("***                      ***");
        zInfo("***  ENTERING TEST MODE  ***");
        zInfo("***                      ***");
        zInfo("****************************");
        FileNameHelper::instance().setTestMode(true);
        //zInfo("testProfile: "+SettingsManager::instance().testProfile());
        TestManager::instance().runBusinessLogicTests(SettingsManager::instance().testProfile());
        return 0;
    }

    QApplication a(argc, argv);
    // 3. QApplication bekötése
    LifecycleManager::instance().setPhase_3(&a);

    StartupManager manager;

    // 🔵 2) Startup pipeline futtatása
    StartupStatus status = manager.runStartupSequence();

    // Opcionális finomhangolás a dialógusokra:
    StartupStatusManager::setPreviewLimit(2);           // hány warning jelenjen meg előnézetben
    StartupStatusManager::setShowLogHint(true);         // mutassuk-e a "részletek a logban" sort
    QString fnh = LogManager::instance().currentFile(LogManager::Channel::Errors);
    //FileNameHelper::instance().getHumanReadableLogFilePath();

    StartupStatusManager::setHumanLogPath(fnh);// emberbarát fájlnév (ha van)
    bool startupOk = StartupStatusManager::handle(status);

    // 🔵 3) Audit
    // 1. már minden registry regisztrálva van
    // 2. és lefutott rajtuk a startup sequence is
    RegistryManager::instance().auditReport();

    if (!startupOk) {
        return -1; // kritikus hiba → leállás
    }

    MainWindow w;

    LifecycleManager::instance().setMainWindow(&w);

    QScreen* scr = QGuiApplication::primaryScreen();
    QObject::connect(scr, &QScreen::geometryChanged, &w, [&w]() {
        if (WindowGeometryHelper::isWindowGeometryReady(&w)) {
            WindowStateManager::instance().saveSnapshot_MainWindow(&w);
            zInfo("💾 Screen geometry changed → instant window snapshot saved");
        } else {
            zInfo("⏳ Screen geometry changed, but window not ready → snapshot skipped");
        }
    });

    // MainWindow megnyílás/bezárás automatikus követése
    LifecycleManager::instance().setPhase_4(&w);
    w.show();
    Logger::setBreakOnError(true);
    int exitCode = a.exec();

    LifecycleManager::instance().setPhase_5(exitCode);

    return exitCode;
}
