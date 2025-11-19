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

int main(int argc, char *argv[])
{
    // induláskor
    SignalHelper::setCleanupHandler([](int sig){
        EventLogger::instance().zEvent_(EventLogger::Info,
                                        QString("🛠️ Alkalmazás leállítása, jel: %1").arg(sig));

        if (sig == SignalHelper::SIGINT_) {
            qDebug().noquote() << "Ctrl+C megszakítás → gyors mentés";
        } else if (sig == SignalHelper::SIGTERM_) {
            qDebug().noquote() << "Killall → teljes cleanup";
        }
        // registry flush, fájlmentés, stb.
    });

    SignalHelper::setShutDownSignal(SignalHelper::SIGINT_); // shut down on ctrl-c
    SignalHelper::setShutDownSignal(SignalHelper::SIGTERM_); // shut down on killall

    QCoreApplication::setApplicationName(SysInfoHelper::instance().target());
    QCoreApplication::setApplicationVersion(Buildnumber::value);
    QCoreApplication::setOrganizationName("horvathzoltan");
    QCoreApplication::setOrganizationDomain("https://github.com/horvathzoltan");

    // itt initelünk mindet - először a loggert hogy tudjunk loggolni
    Logger::Init(Logger::ErrLevel::INFO, Logger::DbgLevel::TRACE, false, false);

    auto sysInfo = SysInfoHelper::instance().sysInfo();
    zInfo(sysInfo);
    zEvent(sysInfo);


    FileNameHelper::setBinaryPath(argv[0]);    
    SettingsManager::instance().detectTestMode(argc, argv);
    FileNameHelper::instance().setDataRootPath(SettingsManager::instance().dataRootPath());

    // --test eventlogger
    if (SettingsManager::instance().isTestMode()) {
        bool ok = TestManager::instance().runBusinessLogicTests(SettingsManager::instance().testProfile());
        return ok ? 0 : 1;
    }

    // 🔧 Eseménynapló fájl megnyitása még az init előtt
    EventLogger::instance().setLogFile("eventlog.txt");


    QApplication a(argc, argv);


    StartupManager manager;
    StartupStatus status = manager.runStartupSequence();

    if (!status.isSuccess())  {
        QMessageBox::critical(nullptr, "Indítási hiba", status.errorMessage());
        return -1;
    }

    if (!status.warnings().isEmpty()) {
        QMessageBox::warning(nullptr, "Figyelmeztetés",
                             "Az alkalmazás elindult, de a következő problémák felmerültek:\n\n" +
                                 status.warnings().join("\n"));
    }

    MainWindow w;
    w.show();
    return a.exec();
}
