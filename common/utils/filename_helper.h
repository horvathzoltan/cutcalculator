#pragma once

#include <QDate>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include "common/logger/logger.h"


enum class FileKind {
    Materials,
    Products,
    NeedRules,
    NeedCalculations,
    NeedCalculationDetails,
    Barcodes,

    RalClassic,
    RalDesign,
    RalPlastic1,
    RalPlastic2,

    UiSnapshotDir,   // könyvtár
    UiSnapshotFile,  // fájl
    LogDir,
    SettingsIni,
    CacheDir,
    MainWindow_SnapshotFile,
    BOM_Workbench_SnapshotFile,
    UIState_SnapshotFile,

    OrderHeaders,
    OrderItems,
    LogArchiveDir
};

enum class FileAccess {
    Read,
    Write
};

enum class FileBehavior {
    Crud,
    ReadOnly,
    Config
};


struct FileKindInfo {
    QString fileName;
    FileBehavior behavior;
};

//3. 🧠 Lehetőség singletonná alakításra
// 📁 Tesztfájl elérési segédfüggvények
class FileNameHelper{
    friend class FileNameHelperTesterProxy;

public:


    QString pathFor(FileKind kind, FileAccess access,const QString& overrideFileName="") const;

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
            QString _containerName = "unknown";


        public:
            RootPathContainer() = delete;

            explicit RootPathContainer(const QString& containerName)
                : _containerName(containerName)
            {}

            void setRootPath(const QString& path, InitSource source) {
                _initSource = source;
                if (_initialized) {
                    zWarning() << "FileNameHelper::setRootPath("+_containerName+"): called twice; ignoring";
                    return;
                }

                _rootPath = QDir::cleanPath(path);
                if (_rootPath.isEmpty()) {
                    zWarning() << "ℹ️ FileNameHelper("+_containerName+"): empty datapath";
                } else {
                    _initialized = true;
                    zInfo().noquote() << "✅ RootPath set("+_containerName+"):" << _rootPath;
                }
            }

            QString filePath(const QString& fileName) const {
                QString msg = "FileNameHelper("+_containerName+")";
                Q_ASSERT_X(_initialized, msg.toStdString().c_str(),"setRootPath(path) must be called");
                if(fileName.isEmpty()) {
                    return _rootPath;
                }

                return QDir(_rootPath).filePath(fileName);
            }

            bool isInitialized() const { return _initialized;}
            InitSource initSource() const { return _initSource; }
            bool isEmpty() const { return _rootPath.isEmpty(); }
    };

    class TestableRootPathContainer : public RootPathContainer {
    private:
        bool _testMode = false;

    public:
        explicit TestableRootPathContainer(const QString& containerName)
            : RootPathContainer(containerName)   // ← továbbadjuk a nevet
        {}

        void setTestMode(bool enabled) {
            _testMode = enabled;
        }

        QString filePath(const QString& fileName) const {
            if (_testMode) {
                // automatikus test mappa a konténer saját rootja alatt
                QString base = RootPathContainer::filePath("");   // saját root
                QDir dir(QDir(base).filePath("test"));

                if (!dir.exists())
                    dir.mkpath(".");

                return dir.filePath(fileName);
            }

            return RootPathContainer::filePath(fileName);
        }
    };


    RootPathContainer _dataRoot_TEST = RootPathContainer("DATA_ROOT_TEST");
    TestableRootPathContainer _dataRoot = TestableRootPathContainer("DATA_ROOT");
    RootPathContainer _dataRoot_MAIN = RootPathContainer("DATA_ROOT_MAIN");

    static RootPathContainer _brc;
    // static QString _binaryPath;
    // static bool _initialized_binaryPath;

    bool _isTest = false;

    // 🛡️ Privát konstruktor
    FileNameHelper();
    bool init(const char* file);
    QString generateTimestamp() const;
    //QString getRalColorsFilePath(const QString& path) const;


    QString configPath(const QString &mainRoot, const QString &fileName) const;
    QString join(const QString &root, const QString &sub) const;
    QString crudRead(const QString &dataRoot, const QString &testRoot, const QString &fileName, bool isTest) const;
    QString makeTestWritePath(const QString &dataRoot, const QString &fileName) const;
public:
    // 🔁 Példány elérése
    static FileNameHelper& instance();

    static void setBinaryPath(const char* argv0);
    static QString binaryPath(){ return _brc.filePath(""); }
    void setDataRootPath(const QString& path);
    //QString dataRootPath() const;

    // ⚙️ Beállítások
    void setTestMode(bool v) {
        _isTest = v;
        _dataRoot.setTestMode(v);
    }

    void setBinaryPath(const QString &a);
    bool isInitialized() const { return _dataRoot_TEST.isInitialized(); }
    bool isTestMode() const { return _isTest; }
};
