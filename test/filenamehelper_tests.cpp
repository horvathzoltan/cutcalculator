#include "filenamehelper_tests.h"
#include "common/utils/filename_helper.h"
#include "common/logger/logger.h"

bool runFileNameHelperTests()
{
    auto& f = FileNameHelper::instance();

    zInfo() << "==============================================";
    zInfo() << "▶️ FileNameHelper – _dataRoot : NORMAL MODE";
    zInfo() << "==============================================";

    f.setTestMode(false);
    // --- Root path containers ---
    zInfo() << "MAIN root: " << f._dataRoot_MAIN.filePath("");
    zInfo() << "DATA root: " << f._dataRoot.filePath("");
    zInfo() << "TEST root: " << f._dataRoot_TEST.filePath("");

    f.setTestMode(true);

    zInfo() << "==============================================";
    zInfo() << "▶️ FileNameHelper – _dataRoot : TEST MODE";
    zInfo() << "==============================================";

    // --- Root path containers ---
    zInfo() << "MAIN root: " << f._dataRoot_MAIN.filePath("");
    zInfo() << "DATA root: " << f._dataRoot.filePath("");
    zInfo() << "TEST root: " << f._dataRoot_TEST.filePath("");


    zInfo() << "==============================================";
    zInfo() << "▶️ FileNameHelper PATH TEST – NORMAL MODE";
    zInfo() << "==============================================";

    f.setTestMode(false);

    // --- Normal mode paths ---
    zInfo() << "materials.csv:           " << f.getMaterialCsvFile();
    zInfo() << "products.csv:            " << f.getProductCsvFile();
    zInfo() << "needrules.csv:           " << f.getNeedRuleCsvFile();
    zInfo() << "needcalculations.csv:    " << f.getNeedCalculationCsvFile();
    zInfo() << "needcalc_details.csv:    " << f.getNeedCalculationDetailCsvFile();
    zInfo() << "barcodes.csv:            " << f.getBarcodeCsvFile();
    zInfo() << "RAL classic:             " << f.getRalClassicCsvFile();
    zInfo() << "RAL design:              " << f.getRalDesignCsvFile();
    zInfo() << "snapshot dir:            " << f.uiSnapshotDirectory();
    zInfo() << "cache dir:               " << f.getCacheDirectory("testcache");

    // --- Switch to test mode ---
    zInfo() << "==============================================";
    zInfo() << "▶️ FileNameHelper PATH TEST – TEST MODE";
    zInfo() << "==============================================";

    f.setTestMode(true);

    // --- Test mode paths ---
    zInfo() << "materials.csv:           " << f.getMaterialCsvFile();
    zInfo() << "products.csv:            " << f.getProductCsvFile();
    zInfo() << "needrules.csv:           " << f.getNeedRuleCsvFile();
    zInfo() << "needcalculations.csv:    " << f.getNeedCalculationCsvFile();
    zInfo() << "needcalc_details.csv:    " << f.getNeedCalculationDetailCsvFile();
    zInfo() << "barcodes.csv:            " << f.getBarcodeCsvFile();
    zInfo() << "snapshot dir:            " << f.uiSnapshotDirectory();
    zInfo() << "cache dir:               " << f.getCacheDirectory("testcache");

    zInfo() << "==============================================";
    zInfo() << "▶️ FileNameHelper PATH TEST – DONE";
    zInfo() << "==============================================";

    return true;
}
