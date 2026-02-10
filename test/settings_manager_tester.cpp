#include "settings_manager_tester.h"
#include "common/utils/filename_helper.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>

void SettingsManagerTester::testFallbackInNormalMode()
{
    zInfo("→ testFallbackInNormalMode");

    auto& sm = SettingsManager::instance();

    // ÉLES mód
    char* argv[] = { (char*)"app", (char*)"--test", (char*)"none" };
    sm.detectTestMode(3, argv);
    Q_ASSERT(!sm.isTestMode());

    // Primary settings.ini törlése → Missing kulcsok
    QString primary = FileNameHelper::instance().pathFor(
        FileKind::SettingsIni, FileAccess::Write
        );
    QFile::remove(primary);

    // Fallback settings.ini létezik
    QString fallback = FileNameHelper::instance().pathFor(
        FileKind::SettingsIni, FileAccess::Read
        );
    Q_ASSERT(QFile::exists(fallback));

    // Missing → fallbackből jön
    QString dp = sm.dataRootPath();
    Q_ASSERT(!dp.isEmpty());

    // ÉLES módban visszaíródik a primary-be
    Q_ASSERT(QFile::exists(primary));
}

void SettingsManagerTester::testNoWriteInTestMode()
{
    zInfo("→ testNoWriteInTestMode");

    auto& sm = SettingsManager::instance();

    // TESZT mód
    char* argv[] = { (char*)"app", (char*)"--test", (char*)"profile1" };
    sm.detectTestMode(3, argv);
    Q_ASSERT(sm.isTestMode());

    QString primary = FileNameHelper::instance().pathFor(
        FileKind::SettingsIni, FileAccess::Write
        );
    QFile::remove(primary);

    // detectTestMode → fallbackből újra létrejön
    QString dp = sm.dataRootPath();
    Q_ASSERT(!dp.isEmpty());
    Q_ASSERT(QFile::exists(primary));   // <-- EZ A HELYES

    // Próbáljunk írni
    sm.setDataPath("SHOULD_NOT_WRITE");
    sm.save();

    // Ellenőrizzük, hogy NEM íródott bele
    QSettings s(primary, QSettings::IniFormat);
    QString stored = s.value("datapath").toString();

    Q_ASSERT(stored != "SHOULD_NOT_WRITE");  // <-- EZ A LÉNYEG
}


void SettingsManagerTester::testFallbackActivationOrder()
{
    zInfo("→ testFallbackActivationOrder");

    auto& sm = SettingsManager::instance();

    // Primary törlése
    QString primary = FileNameHelper::instance().pathFor(
        FileKind::SettingsIni, FileAccess::Write
        );
    QFile::remove(primary);

    // A SettingsManager már inicializálva van → dpBefore NEM üres
    QString dpBefore = sm.dataRootPath();
    Q_ASSERT(!dpBefore.isEmpty());   // <-- EZ A HELYES ELVÁRÁS

    // Most aktiváljuk a fallbacket új profillal
    char* argv[] = { (char*)"app", (char*)"--test", (char*)"profile1" };
    sm.detectTestMode(3, argv);

    QString dpAfter = sm.dataRootPath();
    Q_ASSERT(!dpAfter.isEmpty());
}



void SettingsManagerTester::testMissingKeyBehavior()
{
    zInfo("→ testMissingKeyBehavior");

    auto& sm = SettingsManager::instance();

    // TESZT mód
    char* argv[] = { (char*)"app", (char*)"--test", (char*)"profile1" };
    sm.detectTestMode(3, argv);

    QString primary = FileNameHelper::instance().pathFor(
        FileKind::SettingsIni, FileAccess::Write
        );
    QFile::remove(primary);

    // Missing → fallbackből jön
    QString dp = sm.dataRootPath();
    Q_ASSERT(!dp.isEmpty());

    // Üres string → valid érték, de fallback továbbra is aktív
    sm.setDataPath("");
    QString dp2 = sm.dataRootPath();
    Q_ASSERT(!dp2.isEmpty());  // <-- EZ A HELYES
}


bool SettingsManagerTester::run()
{
    zInfo("=== SettingsManager TESTS START ===");

    QString primary = FileNameHelper::instance().pathFor(
        FileKind::SettingsIni, FileAccess::Write
        );

    QString backup = primary + ".BAK";

    // Ha létezik az éles settings.ini → átnevezzük
    if (QFile::exists(primary)) {
        QFile::remove(backup); // ha maradt régi
        QFile::rename(primary, backup);
    }

    // 🔧 1) FileNameHelper inicializálása (ugyanaz, mint a main-ben)
    auto e1 = FileNameHelper::instance().binaryPath(); //QCoreApplication::applicationFilePath();
    bool isInited = FileNameHelper::instance().isInitialized();
    FileNameHelper::setBinaryPath(e1.toUtf8().constData());

    // 🔧 2) SettingsManager fallback + testmode helyes működéséhez
    // SettingsManager::instance().detectTestMode(0, nullptr);
    // FileNameHelper::instance().setDataRootPath(
    //     SettingsManager::instance().dataRootPath()
    //     );

    // 🔧 3) Most már minden teszt futtatható
    testFallbackInNormalMode();
    testNoWriteInTestMode();
    testFallbackActivationOrder();
    testMissingKeyBehavior();


    // Teszt által létrehozott settings.ini törlése
    if (QFile::exists(primary)) {
        QFile::remove(primary);
    }

    // Eredeti visszaállítása
    if (QFile::exists(backup)) {
        QFile::rename(backup, primary);
    }

    zInfo("=== SettingsManager TESTS END ===");
    return true;
}
