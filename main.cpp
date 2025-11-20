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

#include "common/logger/log_manager.h"

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

    // Induláskor ideiglenes fájlok a bináris mellé
    FileNameHelper::setBinaryPath(argv[0]);
    auto logDir = FileNameHelper::instance().binaryPath();

    // itt most a binary kellene
    // aztán pedig a datapath - a datapath most még üres

    // teháét amíg be a datapath a FileNameHelper-ben üres, addig a binaryba fogunk loggolni
    // aztán pedig a data könyvtárba, a fájlok áthelyezése mellett
    auto file_events = FileNameHelper::instance().getLogFilePath("events");
    auto file_log = FileNameHelper::instance().getLogFilePath("log");

    QString ts = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    LogManager::instance().setFile(LogManager::Channel::Events, logDir + "/events_" + ts + ".txt");
    LogManager::instance().setFile(LogManager::Channel::Errors, logDir + "/log_" + ts + ".txt");

    // Csatornánkénti flush intervallum (pl. errors gyorsabban)
    LogManager::instance().setFlushInterval(LogManager::Channel::Events, 2000);
    LogManager::instance().setFlushInterval(LogManager::Channel::Errors, 2000);

    // Írás
    LogManager::instance().write(LogManager::Channel::Events, "🟢 START application");
    LogManager::instance().write(LogManager::Channel::Errors, "INFO: RootPath beállítva");

    QCoreApplication::setApplicationName(SysInfoHelper::instance().target());
    QCoreApplication::setApplicationVersion(Buildnumber::value);
    QCoreApplication::setOrganizationName("horvathzoltan");
    QCoreApplication::setOrganizationDomain("https://github.com/horvathzoltan");

    // itt initelünk mindet - először a loggert hogy tudjunk loggolni
    Logger::Init(Logger::ErrLevel::INFO, Logger::DbgLevel::TRACE, false, false);

    QString sysInfo = SysInfoHelper::instance().sysInfo();
    zInfo() << sysInfo;
    zEvent(sysInfo);

    // 1. SettingsManager konstruktora átmásolja a settings.ini fájlt a testdata-ból a bináris mellé
    // 2. és felolvassa a beállításokat
    // 3. a bináris patht a  FileNameHelper::instance().getSettingsFilePath(false); -ből szedi
    // ezért azt előtte be kell állítani:     FileNameHelper::setBinaryPath(argv[0]);
    SettingsManager::instance().detectTestMode(argc, argv);
    // itt állítjuk be a datapathot a settings alapján
    FileNameHelper::instance().setDataRootPath(SettingsManager::instance().dataRootPath());

    // Settings betöltése után mozgatás a data könyvtárba
    auto dataDir = FileNameHelper::instance().getLogFolder();
    LogManager::instance().moveFile(LogManager::Channel::Events, dataDir + "/events_" + ts + ".txt");
    LogManager::instance().moveFile(LogManager::Channel::Errors, dataDir + "/log_" + ts + ".txt");

    // --test eventlogger
    if (SettingsManager::instance().isTestMode()) {
        bool ok = TestManager::instance().runBusinessLogicTests(SettingsManager::instance().testProfile());
        return ok ? 0 : 1;
    }

    // 🔧 Eseménynapló fájl megnyitása még az init előtt
    //EventLogger::instance().setLogFile("eventlog.txt");


    QApplication a(argc, argv);

    LogManager::instance().enableBuffering(true);


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
