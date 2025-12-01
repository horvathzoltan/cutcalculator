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
#include "common/registry/registry_manager.h"

int main(int argc, char *argv[])
{
    Logger::setBreakOnError(false);
    Logger::setVerbose(false);
    QString sysInfo = SysInfoHelper::instance().sysInfo();
    zInfo() << sysInfo;
    zEvent(sysInfo);

    // induláskor
    SignalHelper::setCleanupHandler([](int sig){
        LifecycleManager::instance().onAbort(sig);
        // ide jöhetnek a tényleges cleanup műveletek: registry flush, fájlmentés, stb.
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
    FileNameHelper::instance().setDataRootPath(SettingsManager::instance().dataRootPath());

    // Settings betöltése után mozgatás a data könyvtárba
    // Settings után data mappába mozgatás
    const auto dataDir = FileNameHelper::instance().getLogFolder();
    LifecycleManager::instance().setPhase_2(dataDir);

    // --test eventlogger
    if (SettingsManager::instance().isTestMode()) {
        bool ok = TestManager::instance().runBusinessLogicTests(SettingsManager::instance().testProfile());
        return ok ? 0 : 1;
    }

    QApplication a(argc, argv);
    // 3. QApplication bekötése
    LifecycleManager::instance().setPhase_3(&a);

    StartupManager manager;
    StartupStatus status = manager.runStartupSequence();

    RegistryManager::instance().auditReport();

    // Opcionális finomhangolás a dialógusokra:
    StartupStatusManager::setPreviewLimit(2);           // hány warning jelenjen meg előnézetben
    StartupStatusManager::setShowLogHint(true);         // mutassuk-e a "részletek a logban" sort
    StartupStatusManager::setHumanLogPath("errors.txt");// emberbarát fájlnév (ha van)

    if (!StartupStatusManager::handle(status)) {
        return -1; // kritikus hiba → leállás
    }

    RegistryManager::instance().auditReport();

    MainWindow w;
    // MainWindow megnyílás/bezárás automatikus követése
    LifecycleManager::instance().setPhase_4(&w);
    w.show();
    Logger::setBreakOnError(true);
    int exitCode = a.exec();

    LifecycleManager::instance().setPhase_5(exitCode);

    return exitCode;
}
