#pragma once

#include "filenamehelper_tests.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"
#include <QFile>
#include <QDir>

class test_FileNameHelper{
public:
    static QString dataRoot_TEST_filePath(const QString& a){
        return FileNameHelper::instance()._dataRoot_TEST.filePath(a);
    }
    static QString dataRoot_filePath(const QString& a){
        return FileNameHelper::instance()._dataRoot.filePath(a);
    }
    static QString dataRoot_MAIN_filePath(const QString& a){
        return FileNameHelper::instance()._dataRoot_MAIN.filePath(a);
    }
};


static void testBootstrap()
{
    zInfo() << "=== BOOTSTRAP TESTS ===";

    // _dataRoot_TEST must be initialized by ProjectRootLocator
    zInfo() << "TESTDATA root:" << test_FileNameHelper::dataRoot_TEST_filePath("");

    // MAIN root must be set by setDataRootPath() in main()
    zInfo() << "MAIN root:" << test_FileNameHelper::dataRoot_MAIN_filePath("");

    // DATA root initially equals MAIN
    zInfo() << "DATA root:" << test_FileNameHelper::dataRoot_filePath("");
}

static void testRootSwitching()
{
    zInfo() << "=== ROOT SWITCHING TESTS ===";
    auto& f = FileNameHelper::instance();

    f.setTestMode(false);
    zInfo() << "NORMAL MODE:";
    zInfo() << " MAIN:" << test_FileNameHelper::dataRoot_MAIN_filePath("");
    zInfo() << " DATA:" << test_FileNameHelper::dataRoot_filePath("");
    zInfo() << " TEST:" << test_FileNameHelper::dataRoot_TEST_filePath("");

    f.setTestMode(true);
    zInfo() << "TEST MODE:";
    zInfo() << " MAIN:" << test_FileNameHelper::dataRoot_MAIN_filePath("");
    zInfo() << " DATA:" << test_FileNameHelper::dataRoot_filePath("");
    zInfo() << " TEST:" << test_FileNameHelper::dataRoot_TEST_filePath("");
}

static void testMainDataPaths()
{
    zInfo() << "=== MAIN DATA PATH TESTS ===";

    auto& f = FileNameHelper::instance();

    f.setTestMode(false);
    zInfo() << "NORMAL MODE:";
    zInfo() << " materials:" << f.getMaterialCsvFile();
    zInfo() << " RAL classic:" << f.getRalClassicCsvFile();
    zInfo() << " RAL design:"  << f.getRalDesignCsvFile();
    zInfo() << " snapshot:"    << f.uiSnapshotDirectory();
    zInfo() << " cache:"       << f.getCacheDirectory("testcache");

    f.setTestMode(true);
    zInfo() << "TEST MODE (should be identical):";
    zInfo() << " materials:" << f.getMaterialCsvFile();
    zInfo() << " RAL classic:" << f.getRalClassicCsvFile();
    zInfo() << " RAL design:"  << f.getRalDesignCsvFile();
    zInfo() << " snapshot:"    << f.uiSnapshotDirectory();
    zInfo() << " cache:"       << f.getCacheDirectory("testcache");
}

static void testRegistryPaths()
{
    auto& f = FileNameHelper::instance();

    zInfo() << "=== REGISTRY PATH TESTS ===";

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

static void testFallbackLogic()
{
    auto& f = FileNameHelper::instance();

    zInfo() << "=== FALLBACK TESTS ===";

    // Normal mode fallback
    f.setTestMode(false);

    QString prodMain = test_FileNameHelper::dataRoot_MAIN_filePath("products.csv");
    QString prodTest = test_FileNameHelper::dataRoot_TEST_filePath("products.csv");

    // Ensure MAIN file does not exist
    QFile::remove(prodMain);

    zInfo() << "NORMAL MODE fallback → should use testdata:";
    zInfo() << " products:" << f.getProductCsvFile();

    // Test mode fallback
    f.setTestMode(true);

    QString prodTestMode = test_FileNameHelper::dataRoot_filePath("products.csv");
    QFile::remove(prodTestMode);

    zInfo() << "TEST MODE fallback → should NOT fallback:";
    zInfo() << " products:" << f.getProductCsvFile();
}

static void testDirectoryCreation()
{
    auto& f = FileNameHelper::instance();

    zInfo() << "=== DIRECTORY CREATION TESTS ===";

    f.setTestMode(true);

    QString snap = f.uiSnapshotDirectory();
    zInfo() << " snapshot dir:" << snap << " exists:" << QDir(snap).exists();

    QString cache = f.getCacheDirectory("autocreated");
    zInfo() << " cache dir:" << cache << " exists:" << QDir(cache).exists();
}

bool runFileNameHelperTests()
{
    testBootstrap();
    testRootSwitching();
    testMainDataPaths();
    testRegistryPaths();
    testFallbackLogic();
    testDirectoryCreation();

    zInfo() << "All FileNameHelper tests PASSED";
    return true;
}

// bool runFileNameHelperTests()
// {
//     auto& f = FileNameHelper::instance();

//     zInfo() << "==============================================";
//     zInfo() << "▶️ FileNameHelper – _dataRoot : NORMAL MODE";
//     zInfo() << "==============================================";

//     f.setTestMode(false);
//     // --- Root path containers ---
//     zInfo() << "MAIN root: " << f._dataRoot_MAIN.filePath("");
//     zInfo() << "DATA root: " << f._dataRoot.filePath("");
//     zInfo() << "TEST root: " << f._dataRoot_TEST.filePath("");

//     f.setTestMode(true);

//     zInfo() << "==============================================";
//     zInfo() << "▶️ FileNameHelper – _dataRoot : TEST MODE";
//     zInfo() << "==============================================";

//     // --- Root path containers ---
//     zInfo() << "MAIN root: " << f._dataRoot_MAIN.filePath("");
//     zInfo() << "DATA root: " << f._dataRoot.filePath("");
//     zInfo() << "TEST root: " << f._dataRoot_TEST.filePath("");


//     zInfo() << "==============================================";
//     zInfo() << "▶️ FileNameHelper PATH TEST – NORMAL MODE";
//     zInfo() << "==============================================";

//     f.setTestMode(false);

//     // --- Normal mode paths ---
//     zInfo() << "materials.csv:           " << f.getMaterialCsvFile();
//     zInfo() << "products.csv:            " << f.getProductCsvFile();
//     zInfo() << "needrules.csv:           " << f.getNeedRuleCsvFile();
//     zInfo() << "needcalculations.csv:    " << f.getNeedCalculationCsvFile();
//     zInfo() << "needcalc_details.csv:    " << f.getNeedCalculationDetailCsvFile();
//     zInfo() << "barcodes.csv:            " << f.getBarcodeCsvFile();
//     zInfo() << "RAL classic:             " << f.getRalClassicCsvFile();
//     zInfo() << "RAL design:              " << f.getRalDesignCsvFile();
//     zInfo() << "snapshot dir:            " << f.uiSnapshotDirectory();
//     zInfo() << "cache dir:               " << f.getCacheDirectory("testcache");

//     // --- Switch to test mode ---
//     zInfo() << "==============================================";
//     zInfo() << "▶️ FileNameHelper PATH TEST – TEST MODE";
//     zInfo() << "==============================================";

//     f.setTestMode(true);

//     // --- Test mode paths ---
//     zInfo() << "materials.csv:           " << f.getMaterialCsvFile();
//     zInfo() << "products.csv:            " << f.getProductCsvFile();
//     zInfo() << "needrules.csv:           " << f.getNeedRuleCsvFile();
//     zInfo() << "needcalculations.csv:    " << f.getNeedCalculationCsvFile();
//     zInfo() << "needcalc_details.csv:    " << f.getNeedCalculationDetailCsvFile();
//     zInfo() << "barcodes.csv:            " << f.getBarcodeCsvFile();
//     zInfo() << "snapshot dir:            " << f.uiSnapshotDirectory();
//     zInfo() << "cache dir:               " << f.getCacheDirectory("testcache");

//     zInfo() << "==============================================";
//     zInfo() << "▶️ FileNameHelper PATH TEST – DONE";
//     zInfo() << "==============================================";

//     return true;
// }
