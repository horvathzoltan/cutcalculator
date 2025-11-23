#pragma once

#include <QFile>
#include <QSettings>

#include "common/logger/logger.h"

class SettingsStore {
public:
    explicit SettingsStore(){}

    void init(const QString&path){
        // már inicializálva?
        if (_settings) {
            zError("❌ SettingsStore már inicializálva van, reinit nem engedélyezett!");
            return;
        }

        // üres string?
        if (path.isEmpty()) {
            zError("❌ SettingsStore init: üres path nem engedélyezett!");
            return;
        }

        // fájl létezik?
        if (!QFile::exists(path)) {
            zError() << "❌ SettingsStore init: a fájl nem létezik: " << qPrintable(path);
            return;
        }

        // minden rendben → inicializálás

        _settings = std::make_unique<QSettings>(path, QSettings::IniFormat);
        zInfo() << "✅ Settings inicializálva: " << qPrintable(path);
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
    struct Keys {
        static inline const auto DataPath            = "datapath";
        static inline const auto CuttingPlanFileName = "cutting_plan_file_name";
        static inline const auto WindowGeometry      = "window_geometry";
    };

public:
    static SettingsManager& instance();

    void detectTestMode(int argc, char* argv[]);

    QString testProfile() const { return _testProfile; }

    bool isTestMode(){
        if(_testProfile.isEmpty()) return false;
        if(_testProfile.toLower() == "none") return false;
        return true;
    }

    QString dataRootPath() const {
        return _store.value(Keys::DataPath).toString();
    }

    void setDataPath(const QString& path) {
        persist(Keys::DataPath, path);
    }

    //CuttingPlanFileName
    QString cuttingPlanFileName() const{
        return _store.value(Keys::CuttingPlanFileName).toString();
    }

    void setCuttingPlanFileName(const QString& fn) {
        //_cuttingPlan_FileName = fn;
        persist(Keys::CuttingPlanFileName, fn);
    }

    // ablakméret
    void setWindowGeometry(const QByteArray& state) {
        _store.setValue(Keys::WindowGeometry, state);
    }

    QByteArray windowGeometry() const {
        return _store.value(Keys::WindowGeometry).toByteArray();
    }

private:
    SettingsManager();

    SettingsStore _store;
    QString _testProfile = "none";

    void persist(const QString& key, const QString& value);
    void persist(const QString &key, const QByteArray &value);
};

