#include "filenamehelper_tests.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

#include <QFile>
#include <QDir>

// ------------------------------------------------------------
// Proxy for accessing private FileNameHelper paths
// ------------------------------------------------------------
class FileNameHelperTesterProxy {
public:
    static QString dataRoot_TEST(const QString& a) {
        return FileNameHelper::instance()._dataRoot_TEST.filePath(a);
    }
    static QString dataRoot(const QString& a) {
        return FileNameHelper::instance()._dataRoot.filePath(a);
    }
    static QString dataRoot_MAIN(const QString& a) {
        return FileNameHelper::instance()._dataRoot_MAIN.filePath(a);
    }
};

// ------------------------------------------------------------
// Tests
// ------------------------------------------------------------

void FileNameHelperTester::testBootstrap()
{
    zInfo("→ testBootstrap");

    zInfo() << "TESTDATA root:" << FileNameHelperTesterProxy::dataRoot_TEST("");
    zInfo() << "MAIN root:"     << FileNameHelperTesterProxy::dataRoot_MAIN("");
    zInfo() << "DATA root:"     << FileNameHelperTesterProxy::dataRoot("");
}

void FileNameHelperTester::testRootSwitching()
{
    zInfo("→ testRootSwitching");

    auto& f = FileNameHelper::instance();

    f.setTestMode(false);
    zInfo() << "NORMAL MODE:";
    zInfo() << " MAIN:" << FileNameHelperTesterProxy::dataRoot_MAIN("");
    zInfo() << " DATA:" << FileNameHelperTesterProxy::dataRoot("");
    zInfo() << " TEST:" << FileNameHelperTesterProxy::dataRoot_TEST("");

    f.setTestMode(true);
    zInfo() << "TEST MODE:";
    zInfo() << " MAIN:" << FileNameHelperTesterProxy::dataRoot_MAIN("");
    zInfo() << " DATA:" << FileNameHelperTesterProxy::dataRoot("");
    zInfo() << " TEST:" << FileNameHelperTesterProxy::dataRoot_TEST("");
}

void FileNameHelperTester::testMainDataPaths()
{
    zInfo("→ testMainDataPaths");

    auto& f = FileNameHelper::instance();

    f.setTestMode(false);
    zInfo() << "NORMAL MODE:";
    zInfo() << " materials:"   << f.getMaterialCsvFile();
    zInfo() << " RAL classic:" << f.getRalClassicCsvFile();
    zInfo() << " RAL design:"  << f.getRalDesignCsvFile();
    zInfo() << " snapshot:"    << f.uiSnapshotDirectory();
    zInfo() << " cache:"       << f.getCacheDirectory("testcache");

    f.setTestMode(true);
    zInfo() << "TEST MODE:";
    zInfo() << " materials:"   << f.getMaterialCsvFile();
    zInfo() << " RAL classic:" << f.getRalClassicCsvFile();
    zInfo() << " RAL design:"  << f.getRalDesignCsvFile();
    zInfo() << " snapshot:"    << f.uiSnapshotDirectory();
    zInfo() << " cache:"       << f.getCacheDirectory("testcache");
}

void FileNameHelperTester::testRegistryPaths()
{
    zInfo("→ testRegistryPaths");

    auto& f = FileNameHelper::instance();

    f.setTestMode(false);
    zInfo() << "NORMAL MODE:";
    zInfo() << " products:"         << f.getProductCsvFile();
    zInfo() << " needrules:"        << f.getNeedRuleCsvFile();
    zInfo() << " barcodes:"         << f.getBarcodeCsvFile();
    zInfo() << " needcalculations:" << f.getNeedCalculationCsvFile();
    zInfo() << " needcalc_details:" << f.getNeedCalculationDetailCsvFile();

    f.setTestMode(true);
    zInfo() << "TEST MODE:";
    zInfo() << " products:"         << f.getProductCsvFile();
    zInfo() << " needrules:"        << f.getNeedRuleCsvFile();
    zInfo() << " barcodes:"         << f.getBarcodeCsvFile();
    zInfo() << " needcalculations:" << f.getNeedCalculationCsvFile();
    zInfo() << " needcalc_details:" << f.getNeedCalculationDetailCsvFile();
}

void FileNameHelperTester::testFallbackLogic()
{
    zInfo("→ testFallbackLogic");

    auto& f = FileNameHelper::instance();

    // Normal mode fallback
    f.setTestMode(false);

    QString prodMain = FileNameHelperTesterProxy::dataRoot_MAIN("products.csv");
    QFile::remove(prodMain);

    zInfo() << "NORMAL MODE fallback:";
    zInfo() << " products:" << f.getProductCsvFile();

    // Test mode fallback
    f.setTestMode(true);

    QString prodTest = FileNameHelperTesterProxy::dataRoot("products.csv");
    QFile::remove(prodTest);

    zInfo() << "TEST MODE fallback:";
    zInfo() << " products:" << f.getProductCsvFile();
}

void FileNameHelperTester::testDirectoryCreation()
{
    zInfo("→ testDirectoryCreation");

    auto& f = FileNameHelper::instance();
    f.setTestMode(true);

    QString snap = f.uiSnapshotDirectory();
    zInfo() << " snapshot dir:" << snap << " exists:" << QDir(snap).exists();

    QString cache = f.getCacheDirectory("autocreated");
    zInfo() << " cache dir:" << cache << " exists:" << QDir(cache).exists();
}

// ------------------------------------------------------------
// Entry point
// ------------------------------------------------------------

bool FileNameHelperTester::run()
{
    zInfo(QString("=== %1 TESTS START ===").arg(name()));

    testBootstrap();
    testRootSwitching();
    testMainDataPaths();
    testRegistryPaths();
    testFallbackLogic();
    testDirectoryCreation();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
