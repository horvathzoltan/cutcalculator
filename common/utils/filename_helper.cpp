#include "filename_helper.h"
//#include "settingsmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include "project_root_locator.h"

QString FileNameHelper::_binaryPath = "";
bool FileNameHelper::s_bootstrapped = false;

FileNameHelper::FileNameHelper() {
    _initialized = init(__FILE__);
}

bool FileNameHelper::init(const char* file) {
#if defined(STRINGIFY_H) && defined(STRING) && defined(SOURCE_PATH)
    _projectPath = QDir::cleanPath(QString(STRING(SOURCE_PATH)));
    _initialized = true;
    _initSource = CompileTime;
    return true;
#else
    if (!file || !*file) {
        qWarning() << "❌ init() hívás érvénytelen fájlparaméterrel.";
        _initialized = false;
        _initSource = None;
        return false;
    }

    const QString sourcePath = QDir::cleanPath(QString::fromUtf8(file));
    auto outcome = ProjectRootLocator::locateFrom(sourcePath);
    qInfo().noquote() << outcome.diagnosticMessage();

    switch (outcome.result) {
    case ProjectRootLocator::FoundWithTestdata:
        _testdataPath = outcome.testdataPath;
        _initialized = true;
        _initSource = SourceFileHeuristic;
        return true;

    case ProjectRootLocator::FoundWithoutTestdata:
        _initialized = false;
        _initSource = SourceFileHeuristic;
        return false;

    case ProjectRootLocator::NotFound:
        _initialized = false;
        _initSource = None;
        return false;
    }

    // Biztonsági ág – nem érhető el, de auditálható
    _initialized = false;
    _initSource = None;
    return false;
#endif
}

FileNameHelper& FileNameHelper::instance() {
    Q_ASSERT_X(s_bootstrapped, "FileNameHelper",
               "Call setBinaryPath(argv0) in main() before instance()");

#ifndef QT_DEBUG
    if (!s_bootstrapped) {
        qFatal("FileNameHelper::instance: bootstrap missing (setBinaryPath not called)");
    }
#endif

    static FileNameHelper helper;
    return helper;
}

void FileNameHelper::setBinaryPath(const char* argv0) {
    Q_ASSERT(argv0 && *argv0);
    if (!argv0 || !*argv0) {
        qFatal("FileNameHelper::setBinaryPath: invalid argv0");
    }
    if (s_bootstrapped) {
        qWarning() << "FileNameHelper::setBinaryPath called twice; ignoring";
        return;
    }
    _binaryPath = QFileInfo(QString::fromUtf8(argv0)).absolutePath();
    s_bootstrapped = true;
}

QString FileNameHelper::getSettingsFilePath(bool forWrite) {
    Q_ASSERT_X(s_bootstrapped, "FileNameHelper",
               "setBinaryPath(argv0) must be called before getSettingsFilePath");
#ifndef QT_DEBUG
    if (!s_bootstrapped) qFatal("getSettingsFilePath: bootstrap missing");
#endif

    const QString binIni = QDir(_binaryPath).filePath("settings.ini");
    if (forWrite) return binIni;

    if (QFileInfo::exists(binIni)) return binIni;

    const QString testIni = QDir(_testdataPath).filePath("settings.ini");
    if (QFileInfo::exists(testIni)) return testIni;

    return binIni; // create here on first run
}

QString FileNameHelper::getWorkingFolder() const {
    if(!_initialized) return "";
    return _isTest ? _testdataPath : QCoreApplication::applicationDirPath();
}

QString FileNameHelper::getStorageCsvFile() const {
    auto fn = QDir(_testdataPath).filePath("storages.csv");
    return fn;
}

QString FileNameHelper::getMaterialCsvFile() const {
    auto fn = QDir(_testdataPath).filePath("materials.csv");
    return fn;
}

QString FileNameHelper::getGroupCsvFile() const {
    auto fn = QDir(_testdataPath).filePath("materialgroups.csv"); // vagy ahová ténylegesen rakod
    return fn;
}

QString FileNameHelper::getGroupMembersCsvFile() const {
    auto fn = QDir(_testdataPath).filePath("materialgroup_members.csv"); // vagy ahová ténylegesen rakod
    return fn;
}

QString FileNameHelper::getStockCsvFile() const {
    auto fn = QDir(_testdataPath).filePath("stock.csv"); // vagy ahová ténylegesen rakod
    return fn;
}

QString FileNameHelper::getLeftoversCsvFile() const {
    auto fn = QDir(_testdataPath).filePath("leftovers.csv"); // vagy ahová ténylegesen rakod
    return fn;
}

/*QString FileNameHelper::getSettingsFilePath()  {
    //QString exeDir = QCoreApplication::applicationDirPath();
    //QDir dir(exeDir);
    //return dir.filePath("settings.ini");
    return  QDir(_binaryPath).filePath("settings.ini");
}*/





QString FileNameHelper::generateTimestamp() const {
    return QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
}


QString FileNameHelper::combinePath(const QString& folder, const QString& fileName) const {
    return QDir(folder).filePath(fileName);
}


/*log*/

QString FileNameHelper::getNew_LogFileName() const {
    QString fn0 = QStringLiteral("log_%1.txt").arg(generateTimestamp());
    return fn0;
}

QString FileNameHelper::getLogFolder() const {
    auto fn = QDir(_testdataPath).filePath("logs");
    return fn;
}

QString FileNameHelper::getLogFilePath(const QString& fn) const {
    return combinePath(getLogFolder(), fn);
}

/*cuttingplan*/
QString FileNameHelper::getNew_CuttingPlanFileName() const {
    QString fn0 = QStringLiteral("cuttingplan_%1.txt").arg(generateTimestamp());
    return fn0;
}

QString FileNameHelper::getCuttingPlanFolder() const {
    auto fn = QDir(_testdataPath).filePath("cutting_plans");
    return fn;
}

QString FileNameHelper::getCuttingPlanFilePath(const QString fn) const {
    return combinePath(getCuttingPlanFolder(), fn);
}

/*movement*/

QString FileNameHelper::getMovementLogFileNameForDate(const QDate& date) const {
    return date.toString("yyyy-MM-dd") + ".log";
}

QString FileNameHelper::getMovementLogFilePathForDate(const QDate& date) const {
    return combinePath(getLogFolder(), getMovementLogFileNameForDate(date));
}

/*CuttingMachines*/

QString FileNameHelper::getCuttingMachineCsvFile() const {
    return QDir(_testdataPath).filePath("cuttingmachines.csv");
}

QString FileNameHelper::getCuttingMachineMaterialsCsvFile() const {
    return QDir(_testdataPath).filePath("cuttingmachine_materialtypes.csv");
}

/*RAL colors*/

/* RAL Colors */

QString FileNameHelper::getRalColorsFolder() const {
    auto fn = QDir(_testdataPath).filePath("ral_colors");
    return fn;
}

QString FileNameHelper::getRalClassicCsvFile() const {
    return combinePath(getRalColorsFolder(), "classic.csv");
}

QString FileNameHelper::getRalDesignCsvFile() const {
    return combinePath(getRalColorsFolder(), "design.csv");
}

QString FileNameHelper::getRalPlastic1CsvFile() const {
    return combinePath(getRalColorsFolder(), "p1.csv");
}

QString FileNameHelper::getRalPlastic2CsvFile() const {
    return combinePath(getRalColorsFolder(), "p2.csv");
}

