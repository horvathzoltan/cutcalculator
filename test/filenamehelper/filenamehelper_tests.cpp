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
    static QString pathFor(FileKind k, FileAccess a) {
        return FileNameHelper::instance().pathFor(k, a);
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

void FileNameHelperTester::assertPath(FileKind kind,
                       FileAccess access,
                       const QString& label,
                       const QString& expectedFilename)
{
    QString p = FileNameHelperTesterProxy::pathFor(kind, access);
    zInfo() << label <<
            ((access==FileAccess::Read)?L(" (read):"):L(" (write):"))<< p;

    if(expectedFilename.isEmpty()) {
        Q_ASSERT(p.isEmpty());
        return;
    }

    QFileInfo fi(p);
    QString actualFilename = fi.fileName();

    Q_ASSERT(actualFilename == expectedFilename);
}

void FileNameHelperTester::testMainDataPaths()
{
    zInfo("→ testMainDataPaths");

    auto& f = FileNameHelper::instance();

    //
    // NORMAL MODE
    //
    f.setTestMode(false);
    zInfo() << "NORMAL MODE:";

    // READ-ONLY (Materials + RAL)
    assertPath(FileKind::Materials,   FileAccess::Read,  "materials",   "materials.csv");
    assertPath(FileKind::Materials,   FileAccess::Write, "materials",   "");

    assertPath(FileKind::RalClassic,  FileAccess::Read,  "ral_colors",  "classic.csv");
    assertPath(FileKind::RalDesign,   FileAccess::Read,  "ral_colors",  "design.csv");
    assertPath(FileKind::RalPlastic1, FileAccess::Read,  "ral_colors",  "p1.csv");
    assertPath(FileKind::RalPlastic2, FileAccess::Read,  "ral_colors",  "p2.csv");

    // CRUD (Products, NeedRules, NeedCalcs, Barcodes)
    assertPath(FileKind::Products,               FileAccess::Read,  "products",               "products.csv");
    assertPath(FileKind::Products,               FileAccess::Write, "products",               "products.csv");

    assertPath(FileKind::NeedRules,              FileAccess::Read,  "needrules",              "needrules.csv");
    assertPath(FileKind::NeedRules,              FileAccess::Write, "needrules",              "needrules.csv");

    assertPath(FileKind::NeedCalculations,       FileAccess::Read,  "needcalculations",       "needcalculations.csv");
    assertPath(FileKind::NeedCalculations,       FileAccess::Write, "needcalculations",       "needcalculations.csv");

    assertPath(FileKind::NeedCalculationDetails, FileAccess::Read,  "needcalc_details",       "needcalc_details.csv");
    assertPath(FileKind::NeedCalculationDetails, FileAccess::Write, "needcalc_details",       "needcalc_details.csv");

    assertPath(FileKind::Barcodes,               FileAccess::Read,  "barcodes",               "barcodes.csv");
    assertPath(FileKind::Barcodes,               FileAccess::Write, "barcodes",               "barcodes.csv");


    //
    // TEST MODE
    //
    f.setTestMode(true);
    zInfo() << "TEST MODE:";

    // READ-ONLY (Materials + RAL)
    assertPath(FileKind::Materials,   FileAccess::Read,  "materials",   "materials.csv");
    assertPath(FileKind::Materials,   FileAccess::Write, "materials",   "test_materials.csv");

    assertPath(FileKind::RalClassic,  FileAccess::Read,  "ral_colors",  "classic.csv");
    assertPath(FileKind::RalDesign,   FileAccess::Read,  "ral_colors",  "design.csv");
    assertPath(FileKind::RalPlastic1, FileAccess::Read,  "ral_colors",  "p1.csv");
    assertPath(FileKind::RalPlastic2, FileAccess::Read,  "ral_colors",  "p2.csv");

    // CRUD (Products, NeedRules, NeedCalcs, Barcodes)
    assertPath(FileKind::Products,               FileAccess::Read,  "products",               "products.csv");
    assertPath(FileKind::Products,               FileAccess::Write, "products",               "products.csv");

    assertPath(FileKind::NeedRules,              FileAccess::Read,  "needrules",              "needrules.csv");
    assertPath(FileKind::NeedRules,              FileAccess::Write, "needrules",              "needrules.csv");

    assertPath(FileKind::NeedCalculations,       FileAccess::Read,  "needcalculations",       "needcalculations.csv");
    assertPath(FileKind::NeedCalculations,       FileAccess::Write, "needcalculations",       "needcalculations.csv");

    assertPath(FileKind::NeedCalculationDetails, FileAccess::Read,  "needcalc_details",       "needcalc_details.csv");
    assertPath(FileKind::NeedCalculationDetails, FileAccess::Write, "needcalc_details",       "needcalc_details.csv");

    assertPath(FileKind::Barcodes,               FileAccess::Read,  "barcodes",               "barcodes.csv");
    assertPath(FileKind::Barcodes,               FileAccess::Write, "barcodes",               "barcodes.csv");
}

void FileNameHelperTester::testSnapshotpaths()
{
    zInfo("→ testSnapshotPaths");

    auto& f = FileNameHelper::instance();
    f.setTestMode(true);

    //
    // 1) Snapshot directory létrejön
    //
    QString snapDir =
        f.pathFor(FileKind::UiSnapshotDir, FileAccess::Read);

    zInfo() << " snapshot dir:" << snapDir;

    Q_ASSERT(!snapDir.isEmpty());
    Q_ASSERT(QDir(snapDir).exists());

    //
    // 2) Snapshot file path helyes
    //
    QString profile = "testprofile";
    QString snapFile = f.pathFor(FileKind::UiSnapshotFile,
                                 FileAccess::Write,
                                 profile);

    zInfo() << " snapshot file:" << snapFile;

    Q_ASSERT(!snapFile.isEmpty());

    QFileInfo fi(snapFile);

    //
    // 3) A fájl a snapshot könyvtárban van
    //
    Q_ASSERT(fi.dir().absolutePath() == snapDir);

    //
    // 4) A fájlnév helyes
    //
    Q_ASSERT(fi.fileName() == QString("geometry_%1.ini").arg(profile));
}

void FileNameHelperTester::testFallbackLogic()
{
    zInfo("→ testFallbackLogic");

    auto& f = FileNameHelper::instance();
    QString p;

    //
    // NORMAL MODE fallback
    //
    f.setTestMode(false);

    // Töröljük a fő products.csv-t, hogy fallback aktiválódjon
    QString prodMain = FileNameHelperTesterProxy::dataRoot("products/products.csv");
    QFile::remove(prodMain);

    zInfo() << "NORMAL MODE fallback:";
    p = FileNameHelperTesterProxy::pathFor(FileKind::Products, FileAccess::Read);
    zInfo() << "products (read):" << p;

    // Fallback → TEST root
    Q_ASSERT(p.endsWith("products.csv"));
    Q_ASSERT(p.contains("testdata"));   // vagy a pontos TEST root neve

    //
    // TEST MODE fallback (nincs fallback!)
    //
    f.setTestMode(true);

    // Töröljük a test products.csv-t
    QString prodTest = FileNameHelperTesterProxy::dataRoot("products/products.csv");
    QFile::remove(prodTest);

    zInfo() << "TEST MODE fallback:";
    p = FileNameHelperTesterProxy::pathFor(FileKind::Products, FileAccess::Read);
    zInfo() << "products (read):" << p;

    // Test mode → nincs fallback → üres path vagy hibás path
    Q_ASSERT(p.endsWith("products.csv"));
    Q_ASSERT(!p.contains("testdata"));  // nincs fallback
}

void FileNameHelperTester::testDirectoryCreation()
{
    zInfo("→ testDirectoryCreation");

    auto& f = FileNameHelper::instance();
    f.setTestMode(true);

    QString snap = f.pathFor(FileKind::UiSnapshotDir, FileAccess::Read);
    zInfo() << " snapshot dir:" << snap << " exists:" << QDir(snap).exists();

    QString cache = f.pathFor(FileKind::CacheDir, FileAccess::Write, "autocreated");
    zInfo() << " cache dir:" << cache << " exists:" << QDir(cache).exists();
}


void FileNameHelperTester::testLogPaths()
{
    zInfo("→ testLogPaths");

    auto& f = FileNameHelper::instance();
    f.setTestMode(true);

    QString logDir = f.pathFor(FileKind::LogDir, FileAccess::Write);

    zInfo() << " log dir:" << logDir;

    Q_ASSERT(!logDir.isEmpty());
    Q_ASSERT(QDir(logDir).exists());
}

void FileNameHelperTester::testSettingsIni()
{
    zInfo("→ testSettingsIni");

    auto& f = FileNameHelper::instance();
    f.setTestMode(true);

    //
    // 1) WRITE → mindig a bináris mellé
    //
    QString writePath = f.pathFor(FileKind::SettingsIni,
                                  FileAccess::Write);

    zInfo() << " settings.ini WRITE path:" << writePath;

    QFileInfo writeFi(writePath);
    QString binDir = writeFi.path();

    Q_ASSERT(writeFi.fileName() == "settings.ini");
    Q_ASSERT(!binDir.isEmpty());


    //
    // 2) READ → fallback testdata/settings.ini, ha nincs bináris melletti
    //
    QFile::remove(writePath);   // biztosan ne létezzen

    QString readFallback = f.pathFor(FileKind::SettingsIni,
                                     FileAccess::Read);

    zInfo() << " settings.ini READ fallback path:" << readFallback;

    QFileInfo fb(readFallback);
    Q_ASSERT(fb.fileName() == "settings.ini");

    // A fallback pathnak a TESTDATA root alatt kell lennie
    QString expectedFallback = FileNameHelperTesterProxy::dataRoot_TEST("settings.ini");
    Q_ASSERT(readFallback == expectedFallback);


    //
    // 3) READ → ha létezik a bináris melletti settings.ini → azt adja
    //
    QFile tmp(writePath);
    if (!tmp.open(QIODevice::WriteOnly)) {
        zInfo("Nem sikerült megnyitni a temporary fájlt írásra.");
    }

    tmp.close();

    QString readPrimary = f.pathFor(FileKind::SettingsIni,
                                    FileAccess::Read);

    zInfo() << " settings.ini READ primary path:" << readPrimary;

    Q_ASSERT(readPrimary == writePath);
}

void FileNameHelperTester::testCacheDir()
{
    zInfo("→ testCacheDir");

    auto& f = FileNameHelper::instance();
    f.setTestMode(true);

    QString cacheDir = f.pathFor(FileKind::CacheDir,
                                 FileAccess::Write,
                                 "autocreated");

    zInfo() << " cache dir:" << cacheDir;

    Q_ASSERT(!cacheDir.isEmpty());
    Q_ASSERT(QDir(cacheDir).exists());
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
    testFallbackLogic();
    testDirectoryCreation();
    testSnapshotpaths();
    testLogPaths();
    testSettingsIni();
    testCacheDir();

    zInfo(QString("=== %1 TESTS END ===").arg(name()));
    return true;
}
