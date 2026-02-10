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
    { FileKind::Materials,   { "materials.csv", FileBehavior::ReadOnly } },
    { FileKind::RalClassic,  { "ral_colors/classic.csv",  FileBehavior::ReadOnly } },
    { FileKind::RalDesign,   { "ral_colors/design.csv",   FileBehavior::ReadOnly } },
    { FileKind::RalPlastic1, { "ral_colors/p1.csv",        FileBehavior::ReadOnly } },
    { FileKind::RalPlastic2, { "ral_colors/p2.csv",        FileBehavior::ReadOnly } },

    // CRUD
    { FileKind::Products,               { "products.csv",               FileBehavior::Crud } },
    { FileKind::NeedRules,              { "needrules.csv",              FileBehavior::Crud } },
    { FileKind::NeedCalculations,       { "needcalculations.csv",       FileBehavior::Crud } },
    { FileKind::NeedCalculationDetails, { "needcalc_details.csv",       FileBehavior::Crud } },
    { FileKind::Barcodes,               { "barcodes.csv",               FileBehavior::Crud } },

    { FileKind::UiSnapshotDir,  { "ui_snapshots", FileBehavior::Config } },
    { FileKind::UiSnapshotFile, { "ui_snapshots/geometry_%1.ini", FileBehavior::Config } },

    { FileKind::LogDir, { "CutCalculator_logs", FileBehavior::Config } },
    { FileKind::SettingsIni, { "settings.ini", FileBehavior::Config } },
    { FileKind::CacheDir, { "%1_cache", FileBehavior::Config } }
};


QString FileNameHelper::pathFor(FileKind kind, FileAccess access, const QString& overrideName) const
{
    if (!isInitialized()) {
        zWarning("⚠️ FileNameHelper nincs inicializálva.");
        return "";
    }

    // 🔒 Biztonsági ellenőrzés: minden FileKind legyen a FILE_INFO-ban
    if (!FILE_INFO.contains(kind)) {
        zError() << "❌ FileNameHelper::pathFor – unknown FileKind:" << int(kind);
        return "";
    }

    const auto& info = FILE_INFO[kind];
    QString fileName = info.fileName;

    if (!overrideName.isEmpty()) {
        // ha a fileName tartalmaz %1-et → sablonként kezeljük
        if (fileName.contains("%1"))
            fileName = fileName.arg(overrideName);
        else
            fileName = overrideName;
    }

    // Csak CRUD és ReadOnly fájloknál kötelező a dataRoot
    if (_dataRoot.isEmpty() &&
        (info.behavior == FileBehavior::Crud ||
         info.behavior == FileBehavior::ReadOnly)) {

        zWarning("⚠️ FileNameHelper: dataRootPath üres, pathFor nem tud érvényes utat adni.");
        return "";
    }


    if (info.behavior == FileBehavior::Crud) {

        // WRITE → mindig _dataRoot
        if (access == FileAccess::Write)
            return _dataRoot.filePath(fileName);

        // READ
        QString p = _dataRoot.filePath(fileName);

        if (_isTest)
            return p; // no fallback in test mode

        if (QFile::exists(p))
            return p;

        return _dataRoot_TEST.filePath(fileName);
    }

    // READ-ONLY behavior
    if (info.behavior == FileBehavior::ReadOnly) {

        if (access == FileAccess::Read)
            return _dataRoot_MAIN.filePath(fileName);

        if (_isTest)
            return _dataRoot.filePath("test_" + fileName);

        zError() << fileName << " is read-only in normal mode";
        return "";
    }

    if (info.behavior == FileBehavior::Config) {

        if (kind == FileKind::SettingsIni) {
            QString primary = _brc.filePath("settings.ini");
            QString fallbackUser = QDir::home().filePath("CutData/CutCalculator_settings.ini");

            if (access == FileAccess::Write)
                return primary;

            if (QFileInfo::exists(primary))
                return primary;

            if (QFileInfo::exists(fallbackUser))
                return fallbackUser;

            // végső fallback fejlesztés alatt
            return _dataRoot_TEST.filePath("settings.ini");
        }

        // A kitöltött fájlnév
        QString fullPath = _dataRoot_MAIN.filePath(fileName);

        // Csak a könyvtárat hozzuk létre
        QFileInfo fi(fullPath);
        QDir dir(fi.path());

        if (!dir.exists())
            dir.mkpath(".");

        return fullPath;
    }

    return "";
}


