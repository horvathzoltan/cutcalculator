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
        QString a = QString("🛠️ Alkalmazás leállítása, jel: %1").arg(sig);
        EventLogger::instance().zEvent_(EventLogger::Info,a);
        LogManager::instance().flushAll();

        if (sig == SignalHelper::SIGINT_) {
            qDebug().noquote() << "Ctrl+C megszakítás";
        } else if (sig == SignalHelper::SIGTERM_) {
            qDebug().noquote() << "Killall";
        }
        // registry flush, fájlmentés, stb.
    });

    SignalHelper::setShutDownSignal(SignalHelper::SIGINT_); // shut down on ctrl-c
    SignalHelper::setShutDownSignal(SignalHelper::SIGTERM_); // shut down on killall

    // Induláskor ideiglenes fájlok a bináris mellé
    FileNameHelper::setBinaryPath(argv[0]);
    const auto binDir = FileNameHelper::instance().binaryPath();

    // Induláskor: mappát adunk meg (pre‑Qt unbuffered ír)
    // Csak Events és Errors csatornát indítjuk
    LogManager::instance().initChannels(binDir, {LogManager::Channel::Events, LogManager::Channel::Errors});
    // Írás minden aktív csatornába
    LogManager::instance().writeAll("🟢 START application");

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
    // Settings után data mappába mozgatás
    const auto dataDir = FileNameHelper::instance().getLogFolder();
    LogManager::instance().moveToFolder(dataDir);

    // --test eventlogger
    if (SettingsManager::instance().isTestMode()) {
        bool ok = TestManager::instance().runBusinessLogicTests(SettingsManager::instance().testProfile());
        return ok ? 0 : 1;
    }

    // 🔧 Eseménynapló fájl megnyitása még az init előtt
    //EventLogger::instance().setLogFile("eventlog.txt");


    QApplication a(argc, argv);

    LogManager::instance().enableBuffering(true);
    LogManager::instance().setFlushInterval(LogManager::Channel::Events, 2000);
    LogManager::instance().setFlushInterval(LogManager::Channel::Errors, 1000);

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
    int exitCode = a.exec();

    LogManager::instance().writeAll("🛑 STOP application. exit code: " + QString::number(exitCode));
    LogManager::instance().flushAll();

    return exitCode;
}
