#include "filename_helper.h"
#include <QFileInfo>
#include <QCoreApplication>
#include "project_root_locator.h"
#include <QStandardPaths>

FileNameHelper::RootPathContainer FileNameHelper::_brc;

FileNameHelper::FileNameHelper() {init(__FILE__);}

bool FileNameHelper::init(const char* file) {
#if defined(STRINGIFY_H) && defined(STRING) && defined(SOURCE_PATH)
    _tdp.setRootPath(QDir::cleanPath(QString(STRING(SOURCE_PATH))),CompileTime);
    return true;
#else
    if (!file || !*file) {
        zWarning() << "⚠️ init() hívás érvénytelen fájlparaméterrel.";

        return false;
    }

    const QString sourcePath = QDir::cleanPath(QString::fromUtf8(file));
    auto outcome = ProjectRootLocator::locateFrom(sourcePath);
    zInfo().noquote() << outcome.diagnosticMessage();

    switch (outcome.result) {
    case ProjectRootLocator::FoundWithTestdata:
        _dataRoot_TEST.setRootPath(outcome.testdataPath, SourceFileHeuristic);
        return true;

    case ProjectRootLocator::FoundWithoutTestdata:
        _dataRoot_TEST.setRootPath("", SourceFileHeuristic);
        return false;

    case ProjectRootLocator::NotFound:
        return false;
    }
    return false;
#endif
}

FileNameHelper& FileNameHelper::instance() {
    static FileNameHelper helper;
    return helper;
}

void FileNameHelper::setBinaryPath(const QString& a) {
 _brc.setRootPath(a, InitSource::Setter);
}


void FileNameHelper::setBinaryPath(const char* argv0) {
    Q_ASSERT(argv0 && *argv0);
    if (!argv0 || !*argv0) {
        zError("FileNameHelper::setBinaryPath: invalid argv0");
    }    
    _brc.setRootPath(QFileInfo(QString::fromUtf8(argv0)).absolutePath(), InitSource::Setter);
}

// void FileNameHelper::setDataRootPath(const QString& path) {
//     _dataRoot.setRootPath(path, InitSource::Setter);
//     _dataRoot_MAIN.setRootPath(path, InitSource::Setter);
// }

void FileNameHelper::setDataRootPath(const QString& path) {
    QString finalPath = path;

    if (finalPath.isEmpty()) {
        zWarning("⚠️ FileNameHelper::setDataRootPath: üres datapath → fallback ~/CutData");

        finalPath = QDir::home().filePath("CutData");
        QDir().mkpath(finalPath);
    }

    _dataRoot.setRootPath(finalPath, InitSource::Setter);
    _dataRoot_MAIN.setRootPath(finalPath, InitSource::Setter);
}


QString FileNameHelper::generateTimestamp() const {
    return QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
}

static const QMap<FileKind, FileKindInfo> FILE_INFO = {
    // READ-ONLY
    { FileKind::Materials,   { "materials/materials.csv", FileBehavior::ReadOnly } },
    { FileKind::RalClassic,  { "ral_colors/classic.csv",  FileBehavior::ReadOnly } },
    { FileKind::RalDesign,   { "ral_colors/design.csv",   FileBehavior::ReadOnly } },
    { FileKind::RalPlastic1, { "ral_colors/p1.csv",        FileBehavior::ReadOnly } },
    { FileKind::RalPlastic2, { "ral_colors/p2.csv",        FileBehavior::ReadOnly } },

    // CRUD
    { FileKind::Products,               { "products/products.csv",               FileBehavior::Crud } },
    { FileKind::NeedRules,              { "calculations/needrules.csv",              FileBehavior::Crud } },
    { FileKind::NeedCalculations,       { "calculations/needcalculations.csv",       FileBehavior::Crud } },
    { FileKind::NeedCalculationDetails, { "calculations/needcalc_details.csv",       FileBehavior::Crud } },
    { FileKind::Barcodes,               { "unique_ids/barcodes.csv",               FileBehavior::Crud } },

    { FileKind::UiSnapshotDir,  { "ui_snapshots", FileBehavior::Config } },
    { FileKind::UiSnapshotFile, { "ui_snapshots/geometry_%1.ini", FileBehavior::Config } },

    { FileKind::LogDir, { "logs", FileBehavior::Config } },
    { FileKind::SettingsIni, { "settings.ini", FileBehavior::Config } },
    { FileKind::CacheDir,           { "%1_cache", FileBehavior::Config } },
    { FileKind::MainWindow_SnapshotFile, { "ui_snapshots/geometry_%1.ini", FileBehavior::Config } },
    //{ FileKind::BOM_Workbench_SnapshotFile, { "ui_snapshots/bom_workbench/%1.ini", FileBehavior::Config } },
    { FileKind::UIState_SnapshotFile, { "ui_snapshots/ui_state_%1.ini", FileBehavior::Config } },

    { FileKind::OrderHeaders, { "orders/order_headers.csv", FileBehavior::Crud } },
    { FileKind::OrderItems,   { "orders/order_items.csv",   FileBehavior::Crud } }

};

QString FileNameHelper::join(const QString& root, const QString& sub) const {
    return QDir::cleanPath(root + "/" + sub);
}

QString FileNameHelper::crudRead(const QString& dataRoot,
                                 const QString& testRoot,
                                 const QString& fileName,
                                 bool isTest) const
{
    QString p = join(dataRoot, fileName);

    if (isTest)
        return p;

    if (QFile::exists(p))
        return p;

    return join(testRoot, fileName);
}

QString FileNameHelper::makeTestWritePath(const QString& dataRoot,
                                          const QString& fileName) const
{
    QFileInfo fi(fileName);
    QString dir  = fi.path();
    QString base = fi.fileName();
    QString testBase = "test_" + base;

    return dir.isEmpty()
               ? join(dataRoot, testBase)
               : join(join(dataRoot, dir), testBase);
}

QString FileNameHelper::configPath(const QString& mainRoot,
                                   const QString& fileName) const
{
    QString full = join(mainRoot, "CutCalculator/" + fileName);
    QDir(QFileInfo(full).path()).mkpath(".");
    return full;
}

QString FileNameHelper::pathFor(FileKind kind,
                                FileAccess access,
                                const QString& overrideName) const
{
    // 0) Alap ellenőrzések
    if (!isInitialized()) {
        zWarning("⚠️ FileNameHelper nincs inicializálva.");
        return "";
    }

    if (!FILE_INFO.contains(kind)) {
        zError() << "❌ pathFor – unknown FileKind:" << int(kind);
        return "";
    }

    const auto& info = FILE_INFO[kind];
    QString fileName = info.fileName;

    // 1) Template kezelés
    if (!overrideName.isEmpty()) {
        fileName = fileName.contains("%1")
        ? fileName.arg(overrideName)
        : overrideName;
    }

    if (fileName.contains("%1") && overrideName.isEmpty()) {
        zError() << "❌ pathFor – template filename requires overrideName:" << fileName;
        return "";
    }

    // 2) CRUD + ReadOnly
    if (info.behavior == FileBehavior::Crud ||
        info.behavior == FileBehavior::ReadOnly)
    {
        const QString mainRoot = _dataRoot_MAIN.filePath("");
        const QString dataRoot = _dataRoot.filePath("");
        const QString testRoot = _dataRoot_TEST.filePath("");

        // 2/A) READ
        if (access == FileAccess::Read) {

            if (info.behavior == FileBehavior::ReadOnly)
                return join(mainRoot, fileName);

            return crudRead(dataRoot, testRoot, fileName, _isTest);
        }

        // 2/B) WRITE
        if (access == FileAccess::Write) {

            if (info.behavior == FileBehavior::ReadOnly) {

                if (!_isTest) {
                    zError() << fileName << " is read-only; write not allowed in normal mode";
                    return "";
                }

                QString full = makeTestWritePath(dataRoot, fileName);
                QDir(QFileInfo(full).path()).mkpath(".");
                return full;
            }

            QString full = join(dataRoot, fileName);
            QDir(QFileInfo(full).path()).mkpath(".");
            return full;
        }
    }

    // 3) CONFIG behavior
    if (info.behavior == FileBehavior::Config) {

        // 3/A) SettingsIni külön ág
        if (kind == FileKind::SettingsIni) {

            const QString primary = _brc.filePath("settings.ini");
            const QString fallbackUser =
                QDir::home().filePath("CutData/CutCalculator/settings.ini");

            const QString testRoot = _dataRoot_TEST.filePath("");
            const QString testFallback = join(testRoot, "settings.ini");

            if (access == FileAccess::Write)
                return primary;

            if (QFileInfo::exists(primary))
                return primary;

            if (QFileInfo::exists(testFallback))
                return testFallback;

            if (QFileInfo::exists(fallbackUser))
                return fallbackUser;

            return testFallback;
        }

        // 3/B) Minden más Config → MAIN alatt
        const QString mainRoot = _dataRoot_MAIN.filePath("");
        return configPath(mainRoot, fileName);
    }

    return "";
}


// QString FileNameHelper::pathFor(FileKind kind, FileAccess access, const QString& overrideName) const
// {
//     if (!isInitialized()) {
//         zWarning("⚠️ FileNameHelper nincs inicializálva.");
//         return "";
//     }

//     // 🔒 Biztonsági ellenőrzés: minden FileKind legyen a FILE_INFO-ban
//     if (!FILE_INFO.contains(kind)) {
//         zError() << "❌ FileNameHelper::pathFor – unknown FileKind:" << int(kind);
//         return "";
//     }

//     const auto& info = FILE_INFO[kind];
//     QString fileName = info.fileName;

//     if (!overrideName.isEmpty()) {
//         // ha a fileName tartalmaz %1-et → sablonként kezeljük
//         if (fileName.contains("%1"))
//             fileName = fileName.arg(overrideName);
//         else
//             fileName = overrideName;
//     }

//     // 🚨 Általános guard: minden %1 sablonhoz kötelező overrideName
//     if (fileName.contains("%1") && overrideName.isEmpty()) {
//         zError() << "❌ FileNameHelper::pathFor – template filename requires non-empty overrideName. "
//                  << "fileName=" << fileName;
//         return "";
//     }

//     // Csak CRUD és ReadOnly fájloknál kötelező a dataRoot
//     if (_dataRoot.isEmpty() &&
//         (info.behavior == FileBehavior::Crud ||
//          info.behavior == FileBehavior::ReadOnly)) {

//         zWarning("⚠️ FileNameHelper: dataRootPath üres, pathFor nem tud érvényes utat adni.");
//         return "";
//     }


//     if (info.behavior == FileBehavior::Crud) {

//         // WRITE → mindig _dataRoot
//         if (access == FileAccess::Write)
//             return _dataRoot.filePath(fileName);

//         // READ
//         QString p = _dataRoot.filePath(fileName);

//         if (_isTest)
//             return p; // no fallback in test mode

//         if (QFile::exists(p))
//             return p;

//         return _dataRoot_TEST.filePath(fileName);
//     }

//     // READ-ONLY behavior
//     // if (info.behavior == FileBehavior::ReadOnly) {

//     //     if (access == FileAccess::Read)
//     //         return _dataRoot_MAIN.filePath(fileName);

//     //     if (_isTest) {
//     //         QFileInfo fi(fileName);
//     //         QString dir = fi.path();
//     //         QString base = fi.fileName();
//     //         QString testName = dir.isEmpty()
//     //                                ? QString("test_%1").arg(base)
//     //                                : QString("%1/test_%2").arg(dir, base);
//     //         return _dataRoot.filePath(testName);
//     //     }

//     //     zError() << fileName << " is read-only in normal mode";
//     //     return "";
//     // }

//     // READ-ONLY behavior
//     if (info.behavior == FileBehavior::ReadOnly) {

//         if (access == FileAccess::Read)
//             return _dataRoot_MAIN.filePath(fileName);

//         if (_isTest && access == FileAccess::Write) {
//             QFileInfo fi(fileName);
//             QString base = fi.fileName();
//             QString testName = QString("test_%1").arg(base);
//             return _dataRoot.filePath(testName);
//         }

//         zError() << fileName << " is read-only in normal mode";
//         return "";
//     }

//     else if (info.behavior == FileBehavior::Config) {

//         if (kind == FileKind::SettingsIni) {
//             QString primary = _brc.filePath("settings.ini");
//             QString fallbackUser = QDir::home().filePath("CutData/CutCalculator/settings.ini");

//             if (access == FileAccess::Write)
//                 return primary;

//             // if (QFileInfo::exists(primary))
//             //     return primary;

//             // if (QFileInfo::exists(fallbackUser))
//             //     return fallbackUser;

//             // // végső fallback fejlesztés alatt
//             // return _dataRoot_TEST.filePath("settings.ini");

//             if (QFileInfo::exists(primary))
//                 return primary;

//             QString testFallback = _dataRoot_TEST.filePath("settings.ini");
//             if (QFileInfo::exists(testFallback))
//                 return testFallback;

//             if (QFileInfo::exists(fallbackUser))
//                 return fallbackUser;

//             return testFallback;
//         }

//         // A kitöltött fájlnév
//         // QString fullPath = _dataRoot_MAIN.filePath(QString("CutCalculator/%1").arg(fileName));


//         // // Csak a könyvtárat hozzuk létre
//         // // QFileInfo fi(fullPath);
//         // // QDir dir(fi.path());

//         // // if (!dir.exists())
//         // //     dir.mkpath(".");

//         // QFileInfo fi(fullPath);

//         // if (fi.isDir()) {
//         //     QDir dir(fullPath);
//         //     if (!dir.exists())
//         //         dir.mkpath(".");
//         // } else {
//         //     QDir dir(fi.path());
//         //     if (!dir.exists())
//         //         dir.mkpath(".");
//         // }

// //        return fullPath;

//         // A kitöltött fájlnév
//         QString fullPath = _dataRoot_MAIN.filePath(QString("CutCalculator/%1").arg(fileName));

//         if (kind == FileKind::CacheDir) {
//             QDir dir(fullPath);
//             if (!dir.exists())
//                 dir.mkpath(".");
//             return fullPath;
//         }

//         QFileInfo fi(fullPath);

//         if (fi.isDir()) {
//             QDir dir(fullPath);
//             if (!dir.exists())
//                 dir.mkpath(".");
//         } else {
//             QDir dir(fi.path());
//             if (!dir.exists())
//                 dir.mkpath(".");
//         }



//         return fullPath;
//     }

//     return "";
// }


// QString FileNameHelper::pathFor(FileKind kind,
//                                 FileAccess access,
//                                 const QString& overrideName) const
// {
//     //
//     // 0) Alap érvényességi ellenőrzések
//     //
//     if (!isInitialized()) {
//         zWarning("⚠️ FileNameHelper nincs inicializálva.");
//         return "";
//     }

//     if (!FILE_INFO.contains(kind)) {
//         zError() << "❌ pathFor – unknown FileKind:" << int(kind);
//         return "";
//     }

//     const auto& info = FILE_INFO[kind];
//     QString fileName = info.fileName;

//     //
//     // 1) Template (%1) kezelés
//     //
//     if (!overrideName.isEmpty()) {
//         if (fileName.contains("%1"))
//             fileName = fileName.arg(overrideName);
//         else
//             fileName = overrideName;
//     }

//     if (fileName.contains("%1") && overrideName.isEmpty()) {
//         zError() << "❌ pathFor – template filename requires overrideName:" << fileName;
//         return "";
//     }

//     //
//     // 2) CRUD + ReadOnly – domain‑tükrözött adatfájlok
//     //
//     if (info.behavior == FileBehavior::Crud ||
//         info.behavior == FileBehavior::ReadOnly)
//     {
//         const QString mainRoot = _dataRoot_MAIN.filePath("");
//         const QString dataRoot = _dataRoot.filePath("");
//         const QString testRoot = _dataRoot_TEST.filePath("");

//         //
//         // 2/A) READ
//         //
//         if (access == FileAccess::Read) {

//             // READ-ONLY → mindig MAIN
//             if (info.behavior == FileBehavior::ReadOnly)
//                 return QDir::cleanPath(mainRoot + "/" + fileName);

//             // CRUD READ
//             QString p = QDir::cleanPath(dataRoot + "/" + fileName);

//             if (_isTest)
//                 return p;

//             if (QFile::exists(p))
//                 return p;

//             return QDir::cleanPath(testRoot + "/" + fileName);
//         }

//         //
//         // 2/B) WRITE
//         //
//         if (access == FileAccess::Write) {

//             //
//             // READ-ONLY WRITE → csak TEST módban engedett
//             //
//             if (info.behavior == FileBehavior::ReadOnly) {

//                 if (!_isTest) {
//                     zError() << fileName << " is read-only; write not allowed in normal mode";
//                     return "";
//                 }

//                 // TEST MODE → test_ prefix, domain‑tükrözve
//                 QFileInfo fi(fileName);
//                 QString dir  = fi.path();       // pl. "materials"
//                 QString base = fi.fileName();   // pl. "materials.csv"
//                 QString testBase = "test_" + base;

//                 QString full = dir.isEmpty()
//                                    ? QDir::cleanPath(dataRoot + "/" + testBase)
//                                    : QDir::cleanPath(dataRoot + "/" + dir + "/" + testBase);

//                 QDir(QFileInfo(full).path()).mkpath(".");
//                 return full;
//             }

//             //
//             // CRUD WRITE → domain‑tükrözött
//             //
//             QString full = QDir::cleanPath(dataRoot + "/" + fileName);
//             QDir(QFileInfo(full).path()).mkpath(".");
//             return full;
//         }
//     }

//     //
//     // 3) CONFIG behavior
//     //
//     if (info.behavior == FileBehavior::Config) {

//         //
//         // 3/A) SettingsIni – speciális, NEM nyúlunk _dataRoot_MAIN‑hez
//         //
//         if (kind == FileKind::SettingsIni) {

//             const QString primary = _brc.filePath("settings.ini");
//             const QString fallbackUser =
//                 QDir::home().filePath("CutData/CutCalculator/settings.ini");

//             const QString testRoot = _dataRoot_TEST.filePath("");
//             const QString testFallback = QDir::cleanPath(testRoot + "/settings.ini");

//             if (access == FileAccess::Write)
//                 return primary;

//             if (QFileInfo::exists(primary))
//                 return primary;

//             if (QFileInfo::exists(testFallback))
//                 return testFallback;

//             if (QFileInfo::exists(fallbackUser))
//                 return fallbackUser;

//             return testFallback;
//         }

//         //
//         // 3/B) Minden más Config → MAIN alatt
//         //
//         const QString mainRoot = _dataRoot_MAIN.filePath("");
//         const QString basePrefix = "CutCalculator";

//         QString full = QDir::cleanPath(mainRoot + "/" + basePrefix + "/" + fileName);

//         QFileInfo fi(full);
//         QDir dir(fi.isDir() ? full : fi.path());
//         if (!dir.exists())
//             dir.mkpath(".");

//         return full;
//     }

//     //
//     // 4) Ismeretlen behavior
//     //
//     return "";
// }
