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

    // Missing → fallbackből jön
    QString dp = sm.dataRootPath();
    Q_ASSERT(!dp.isEmpty());

    // Teszt módban NEM ír vissza
    Q_ASSERT(!QFile::exists(primary));

    // Próbáljunk írni
    sm.setDataPath("SHOULD_NOT_WRITE");
    sm.save();

    // Még mindig nem létezik
    Q_ASSERT(!QFile::exists(primary));
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

    // detectTestMode() előtt fallback NEM aktív
    // (mert a konstruktorban már nem hívjuk meg a setFallback-et)
    // → Missing kulcs esetén üres értéket kell kapnunk
    QString dpBefore = sm.dataRootPath();
    Q_ASSERT(dpBefore.isEmpty());

    // Most aktiváljuk a fallbacket
    char* argv[] = { (char*)"app", (char*)"--test", (char*)"profile1" };
    sm.detectTestMode(3, argv);

    // detectTestMode() után fallback aktív
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

    // Üres string → valid érték, nem pótoljuk
    sm.setDataPath("");
    QString dp2 = sm.dataRootPath();
    Q_ASSERT(dp2.isEmpty());
}


bool SettingsManagerTester::run()
{
    zInfo("=== SettingsManager TESTS START ===");

    // 🔧 1) FileNameHelper inicializálása (ugyanaz, mint a main-ben)
    auto e1 = FileNameHelper::instance().binaryPath(); //QCoreApplication::applicationFilePath();
    bool isInited = FileNameHelper::instance().isInitialized();
    FileNameHelper::setBinaryPath(e1.toUtf8().constData());

    // 🔧 2) SettingsManager fallback + testmode helyes működéséhez
    SettingsManager::instance().detectTestMode(0, nullptr);
    FileNameHelper::instance().setDataRootPath(
        SettingsManager::instance().dataRootPath()
        );

    // 🔧 3) Most már minden teszt futtatható
    testFallbackInNormalMode();
    testNoWriteInTestMode();
    testFallbackActivationOrder();
    testMissingKeyBehavior();

    zInfo("=== SettingsManager TESTS END ===");
    return true;
}
