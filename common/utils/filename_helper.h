#pragma once

#include <QDate>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include "common/logger/logger.h"

//3. 🧠 Lehetőség singletonná alakításra
// 📁 Tesztfájl elérési segédfüggvények
class FileNameHelper{
public:
    enum InitSource { None, CompileTime, SourceFileHeuristic, AppDirFallback , Setter};
    //InitSource initSource() const { return _initSource_testdataPath; }
    //    bool isInitialized() const { return _initialized_testDataPath; }
private:
    //QString _testdataPath;
    //bool _initialized_testDataPath = false;
    //InitSource _initSource_testdataPath = None;

    struct RootPathContainer{
        private:
            QString _rootPath;
            bool _initialized = false;
            InitSource _initSource = None;

        public:
            void setRootPath(const QString& path, InitSource source ) {
                _initSource = source;
                if (_initialized) {
                    zWarning() << "FileNameHelper::setRootPath called twice; ignoring";
                    return;
                }

                _rootPath = QDir::cleanPath(path);
                if (_rootPath.isEmpty()) {
                    zWarning() << "ℹ️ FileNameHelper: empty datapath";
                } else {
                    _initialized = true;
                    zInfo().noquote() << "✅ RootPath set:" << _rootPath;
                }
            }

            QString filePath(const QString& fileName) const {
                Q_ASSERT_X(_initialized, "FileNameHelper","setRootPath(path) must be called");
                if(fileName.isEmpty()) {
                    return _rootPath;
                }

                return QDir(_rootPath).filePath(fileName);
            }

            bool isInitialized() const { return _initialized;}
            InitSource initSource() const { return _initSource; }
            bool isEmpty() const { return _rootPath.isEmpty(); }
    };

    RootPathContainer _dataRoot_TEST;

    RootPathContainer _dataRoot;

    static RootPathContainer _brc;
    // static QString _binaryPath;
    // static bool _initialized_binaryPath;

    bool _isTest = false;

    // 🛡️ Privát konstruktor
    FileNameHelper();
    bool init(const char* file);
    QString generateTimestamp() const;
    QString getRalColorsFilePath(const QString& path) const;
public:
    // 🔁 Példány elérése
    static FileNameHelper& instance();

    static void setBinaryPath(const char* argv0);
    static QString binaryPath(){ return _brc.filePath(""); }
    void setDataRootPath(const QString& path);
    //QString dataRootPath() const;

    bool isTestMode() const { return _isTest; }
    bool isInitialized() const { return _dataRoot_TEST.isInitialized(); }

    // ⚙️ Beállítások
    //void setTestMode(bool v) { _isTest = v; }

    // 📁 Elérési utak
    //QString getTestFolderPath() const;
    //QString getWorkingFolder() const;
    QString getMaterialCsvFile() const; // 📁 Anyag törzs CSV útvonal

    // 📓 Naplófájl név
    //QString getNew_LogFileName() const;
    QString getLogFolder() const;
    //QString getLogFilePath(const QString& fn) const;

    //
    QString getRalClassicCsvFile() const;
    QString getRalDesignCsvFile() const;
    QString getRalPlastic1CsvFile() const;
    QString getRalPlastic2CsvFile() const;

    QString getSettingsFilePath(bool forWrite);
    void setBinaryPath(const QString &a);
    QString getProductCsvFile() const;
    QString getNeedRuleCsvFile() const;
    QString getNeedCalculationCsvFile() const;
    QString getNeedCalculationDetailCsvFile() const;
    QString getBarcodeCsvFile() const;
};
