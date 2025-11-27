#include "filename_helper.h"
#include <QFileInfo>
#include <QCoreApplication>
#include "project_root_locator.h"


FileNameHelper::RootPathContainer FileNameHelper::_brc;

FileNameHelper::FileNameHelper() {init(__FILE__);}

bool FileNameHelper::init(const char* file) {
#if defined(STRINGIFY_H) && defined(STRING) && defined(SOURCE_PATH)
    _tdp.setRootPath(QDir::cleanPath(QString(STRING(SOURCE_PATH))),CompileTime);
    return true;
#else
    if (!file || !*file) {
        zWarning() << "❌ init() hívás érvénytelen fájlparaméterrel.";
        //_initialized_testDataPath = false;
        //_initSource_testdataPath = None;
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
//Q_ASSERT_X(_brc.isInitialized(), "FileNameHelper", "Call setBinaryPath(argv0) in main() before instance()");

// #ifndef QT_DEBUG
//     if (!_brc.isInitialized()) {
//         qFatal("FileNameHelper::instance: bootstrap missing (setBinaryPath not called)");
//     }
// #endif

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

void FileNameHelper::setDataRootPath(const QString& path) {
    _dataRoot.setRootPath(path, InitSource::Setter);
}

QString FileNameHelper::getSettingsFilePath(bool forWrite) {
    Q_ASSERT_X(_brc.isInitialized(), "FileNameHelper","setBinaryPath(argv0) must be called before getSettingsFilePath");
#ifndef QT_DEBUG
    if (!_brc.isInitialized()) qFatal("getSettingsFilePath: bootstrap missing");
#endif

    const QString binIni = _brc.filePath("settings.ini");
    if (forWrite) return binIni;

    if (QFileInfo::exists(binIni)) return binIni;

    if(_dataRoot_TEST.isEmpty()) return "";
    const QString testIni = _dataRoot_TEST.filePath("settings.ini");
    return testIni;
}

QString FileNameHelper::generateTimestamp() const {
    return QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
}

/*log*/

// QString FileNameHelper::getNew_LogFileName() const {
//     QString fn0 = QStringLiteral("log_%1.txt").arg(generateTimestamp());
//     return fn0;
// }

QString FileNameHelper::getLogFolder() const {
     // bináris neve (pl. CutCalculator)
     QString programName = "CutCalculator";//QFileInfo(QCoreApplication::applicationFilePath()).baseName();
     return _dataRoot.filePath(programName + "_logs");
}

// QString FileNameHelper::getLogFilePath(const QString& fn) const {
//     return QDir(getLogFolder()).filePath(fn);
// }

/* Material */
QString FileNameHelper::getMaterialCsvFile() const {
    return _dataRoot.filePath("materials.csv");
}

/* Product */
QString FileNameHelper::getProductCsvFile() const {
    QString path = _dataRoot.filePath("products.csv");

    if (!QFile::exists(path)) {
        // fallback fejlesztéshez
        path = _dataRoot_TEST.filePath("products.csv");
    }
        return path;
}

/* RAL Colors */

QString FileNameHelper::getRalColorsFilePath(const QString& path) const {
    return QDir(_dataRoot.filePath("ral_colors")).filePath(path);
}

QString FileNameHelper::getRalClassicCsvFile() const {
    return getRalColorsFilePath("classic.csv");
}

QString FileNameHelper::getRalDesignCsvFile() const {
    return getRalColorsFilePath("design.csv");
}

QString FileNameHelper::getRalPlastic1CsvFile() const {
    return getRalColorsFilePath("p1.csv");
}

QString FileNameHelper::getRalPlastic2CsvFile() const {
    return getRalColorsFilePath("p2.csv");
}

