#pragma once
#include "common/utils/filename_helper.h"
#include "test/common/test_module_base.h"
#include <QString>

class FileNameHelperTester : public TestModuleBase {
public:
    FileNameHelperTester()
        : TestModuleBase("FileNameHelper")
    {}

    bool run() override;

private:
    // tests
    void testBootstrap();
    void testRootSwitching();
    void testMainDataPaths();
    void testSnapshotpaths();
    void testFallbackLogic();
    void testDirectoryCreation();
    void testLogPaths();
    void testSettingsIni();
    void testCacheDir();

    static void assertPath(FileKind kind,
                           FileAccess access,
                           const QString& label,
                           const QString& expectedFilename);
};
