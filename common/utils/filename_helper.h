#pragma once

#include <QDate>
#include <QFileInfo>
#include <QString>

//3. 🧠 Lehetőség singletonná alakításra
// 📁 Tesztfájl elérési segédfüggvények
class FileNameHelper{
public:
    enum InitSource { None, CompileTime, SourceFileHeuristic, AppDirFallback };
    InitSource initSource() const { return _initSource; }
        bool isInitialized() const { return _initialized; }
private:
    QString _testdataPath;
    bool _isTest = false;
    //bool _isInited = false;

    static QString _binaryPath;
    bool _initialized = false;
    InitSource _initSource = None;
    static bool s_bootstrapped;

    // 🛡️ Privát konstruktor
    FileNameHelper();
    bool init(const char* file);
    QString generateTimestamp() const;
    QString combinePath(const QString &folder, const QString &fileName) const;
    QString getRalColorsFolder() const;
public:
    // 🔁 Példány elérése
    static FileNameHelper& instance();


    static void setBinaryPath(const char* argv0);
    bool isTestMode() const { return _isTest; }
    //bool isInited() const { return _isInited; }

    // ⚙️ Beállítások
    void setTestMode(bool v) { _isTest = v; }

    // 📁 Elérési utak
    //QString getTestFolderPath() const;
    QString getWorkingFolder() const;
    QString getStorageCsvFile() const; // 📁 Tároló CSV útvonal
    QString getMaterialCsvFile() const; // 📁 Anyag törzs CSV útvonal
    QString getGroupCsvFile() const; // 📁 Csoportdefiníciós CSV útvonal
    QString getStockCsvFile() const; // 📁 Készlet CSV útvonal

    // 📓 Naplófájl név
    QString getNew_LogFileName() const;
    QString getLogFolder() const;
    QString getLogFilePath(const QString& fn) const;

    //vágási terv
    QString getNew_CuttingPlanFileName() const;
    QString getCuttingPlanFolder() const;
    QString getCuttingPlanFilePath(const QString fn) const;

    //QString getCuttingCsvFile() const; // ✂️ ÚJ

    // maradékék stock fájlnév
    QString getLeftoversCsvFile() const;


    QString getMovementLogFilePathForDate(const QDate &date) const;
    QString getMovementLogFileNameForDate(const QDate &date) const;
    QString getGroupMembersCsvFile() const;

    //
    QString getCuttingMachineCsvFile() const;
    QString getCuttingMachineMaterialsCsvFile() const;
    QString getRalClassicCsvFile() const;
    QString getRalDesignCsvFile() const;
    QString getRalPlastic1CsvFile() const;
    QString getRalPlastic2CsvFile() const;
    //QString getRalEffectCsvFile() const;
    //Settings fájlnév bin/exe mellett, vagy testfolderben
    //static QString getSettingsFilePath();
    QString getSettingsFilePath(bool forWrite);
};
