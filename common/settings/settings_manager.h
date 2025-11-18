#pragma once

//#include "../model/cutting/optimizer/targetheuristic.h"
#include <QFile>
#include <QSettings>

// namespace SettingsKeys {
// inline constexpr auto CuttingPlanFileName = "cutting_plan_file_name";

// inline constexpr auto DataPath = "datapath";
// // oszlopméret
// // inline constexpr auto TableInputHeader       = "table_input_header";
// // inline constexpr auto TableResultsHeader     = "table_results_header";
// // inline constexpr auto TableStockHeader       = "table_stock_header";
// // inline constexpr auto TableLeftoversHeader   = "table_leftovers_header";
// // inline constexpr auto TableStorageAuditHeader   = "table_storageaudit_header";
// // inline constexpr auto TableRelocationOrderHeader   = "table_relocationorder_header";
// // inline constexpr auto CuttingInstructionTableHeader   = "table_cuttinginstruction_header";

// // ablakméret
// inline constexpr auto WindowGeometry = "window_geometry";
// // splitter
// //inline constexpr auto MainSplitterState = "main_splitter_state";

// // CuttingStrategy
// //inline constexpr auto CuttingStrategy = "cutting_strategy";

// // optimize számlálók
// //inline constexpr auto MaterialCounter = "material_counter";
// //inline constexpr auto LeftoverCounter = "leftover_counter";

// }

class SettingsStore {
public:
    explicit SettingsStore(){}

    void init(const QString&path){
        // már inicializálva?
        if (_settings) {
            qFatal("❌ SettingsStore már inicializálva van, reinit nem engedélyezett!");
            return;
        }

        // üres string?
        if (path.isEmpty()) {
            qFatal("❌ SettingsStore init: üres path nem engedélyezett!");
            return;
        }

        // fájl létezik?
        if (!QFile::exists(path)) {
            qFatal("❌ SettingsStore init: a fájl nem létezik: %s",
                   qPrintable(path));
            return;
        }

        // minden rendben → inicializálás

        _settings = std::make_unique<QSettings>(path, QSettings::IniFormat);
        qInfo("✅ Settings inicializálva: %s", qPrintable(path));
    }

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const {
        return _settings?_settings->value(key, defaultValue):defaultValue;
    }

    void setValue(const QString& key, const QVariant& value) {
        if(_settings)
            _settings->setValue(key, value);
    }

    void sync() {if(_settings) _settings->sync(); }

    bool isInitialized() const {
        return _settings != nullptr;
    }

private:
    std::unique_ptr<QSettings> _settings = nullptr;;
};


class SettingsManager {
public:
    static SettingsManager& instance();

    //void save();
    void detectTestMode(int argc, char* argv[]);

    QString testProfile() const { return _testProfile; }

    bool isTestMode(){
        if(_testProfile.isEmpty()) return false;
        if(_testProfile.toLower() == "none") return false;
        return true;
    }

private:
    static inline const auto DataPath = "datapath";
public:
    QString dataPath() const {
        return _store.value(DataPath).toString();
    }

    void setDataPath(const QString& path) {
        persist(DataPath, path);
    }

private:
    static inline const auto CuttingPlanFileName = "cutting_plan_file_name";
public:
    //CuttingPlanFileName
    QString cuttingPlanFileName() const{
        return _store.value(CuttingPlanFileName).toString();
    }

    void setCuttingPlanFileName(const QString& fn) {
        //_cuttingPlan_FileName = fn;
        persist(CuttingPlanFileName, fn);
    }


private:
    static inline const auto WindowGeometry = "window_geometry";
public:
    // ablakméret
    void setWindowGeometry(const QByteArray& state) {
        _store.setValue(WindowGeometry, state);
    }

    QByteArray windowGeometry() const {
        return _store.value(WindowGeometry).toByteArray();
    }

private:
    SettingsManager();
    //QSettings _settings;
    //std::unique_ptr<QSettings> _settings;
    SettingsStore _store;
    QString _testProfile = "none";

    void persist(const QString& key, const QString& value);
    void persist(const QString &key, const QByteArray &value);

  //  void detectTestMode_(int argc, char *argv[]);
};

