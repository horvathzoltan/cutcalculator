#pragma once

#include <QFile>
#include <QSettings>

#include "common/logger/logger.h"

/**
 * @class SettingsStore
 * @brief Vékony wrapper a QSettings köré, késleltetett inicializálással.
 *
 * Felelősség:
 * - A nyers QSettings példány kezelését végzi (init, value, setValue, sync).
 * - Késleltetett inicializálás: csak akkor hozza létre a QSettings objektumot,
 *   ha ténylegesen létező és érvényes settings.ini fájl elérhető.
 * - Auditbarát hibakezelést biztosít: ha a fájl hiányzik vagy újrainicializálás
 *   történne, explicit logbejegyzést ír.
 *
 * Használat:
 * - A SettingsManager hívja meg az init(path) metódust a megfelelő settings.ini fájlra.
 * - A felsőbb rétegek nem közvetlenül a QSettings-et használják, hanem ezen a rétegen keresztül.
 *
 * Előnyök:
 * - Tesztelhetőség: könnyen mockolható/stubolható.
 * - Jövőbeli bővíthetőség: ha más storage backendre váltanánk (JSON, DB, titkosított fájl),
 *   csak ezt az osztályt kell átírni.
 */
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

    void setValue_persistent(const QString& key, const QString& value);
    void setValue_persistent(const QString &key, const QByteArray &value);

private:
    std::unique_ptr<QSettings> _settings = nullptr;;
};


/**
 * @class SettingsManager
 * @brief Magasabb szintű API a beállításokhoz, konkrét kulcsokkal és logikával.
 *
 * Felelősség:
 * - A SettingsStore rétegen keresztül kezeli a QSettings-et.
 * - Magas szintű getter/setter metódusokat ad a konkrét kulcsokhoz:
 *   pl. windowGeometry(), setCuttingPlanFileName(), currentTabIndex().
 * - Késleltetett inicializálás: a konstruktorban ellenőrzi és előkészíti a settings.ini fájlt
 *   (másolás testdata-ból, audit log, hibakezelés).
 * - Teszt mód detektálása: parancssori argumentumok vagy ini kulcs alapján.
 *
 * Használat:
 * - Singletonként érhető el: SettingsManager::instance().
 * - A GUI komponensek (MainWindow, táblák) ezen keresztül mentik és állítják vissza
 *   az állapotukat.
 *
 * Előnyök:
 * - Egységes API: a felsőbb rétegek nem kulcsokat és QSettings-et használnak,
 *   hanem típusbiztos metódusokat.
 * - Auditbarát: minden inicializálási és hibakezelési lépés explicit logolva van.
 * - Későbbi bővíthetőség: új kulcsokhoz csak új getter/setter metódust kell adni.
 */

class SettingsManager {
    struct Keys {
        static inline const auto DataPath            = "datapath";
        static inline const auto CuttingPlanFileName = "cutting_plan_file_name";
        static inline const auto WindowGeometry      = "window_geometry";
        static inline const auto MainSplitterState      = "main_splitter_state";
        static inline const auto CurrentTabIndex      = "current_tab_index";
        static inline const auto ProductTreeHeaderState      = "product_tree_header_state";
        static inline const auto ProductTypesSplitterState      = "product_types_splitter_state";

    };

public:
    static SettingsManager& instance();

    void detectTestMode(int argc, char* argv[]);

    void save(){_store.sync();}

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
        _store.setValue_persistent(Keys::DataPath, path);
    }

    //CuttingPlanFileName
    QString cuttingPlanFileName() const{
        return _store.value(Keys::CuttingPlanFileName).toString();
    }

    void setCuttingPlanFileName(const QString& fn) {
        //_cuttingPlan_FileName = fn;
        _store.setValue_persistent(Keys::CuttingPlanFileName, fn);
    }

    // ablakméret
    void setWindowGeometry(const QByteArray& state) {
        _store.setValue(Keys::WindowGeometry, state);
    }

    QByteArray windowGeometry() const {
        return _store.value(Keys::WindowGeometry).toByteArray();
    }

    // Splitter state
    void setMainSplitterState(const QByteArray& state) {
        _store.setValue(Keys::MainSplitterState, state);
    }

    QByteArray mainSplitterState() const {
        return _store.value(Keys::MainSplitterState).toByteArray();
    }

    // Current tab
    void setCurrentTabIndex(int i) {
        _store.setValue(Keys::CurrentTabIndex, i);
    }

    int currentTabIndex() const {
        return _store.value(Keys::CurrentTabIndex).toInt();
    }

    QByteArray productTreeHeaderState() const {
        return _store.value(Keys::ProductTreeHeaderState).toByteArray();
    }

    void setProductTreeHeaderState(const QByteArray& state) {
        _store.setValue(Keys::ProductTreeHeaderState, state);
    }

    QByteArray productTypesSplitterState() const {
        return _store.value(Keys::ProductTypesSplitterState).toByteArray();
    }

    void setProductTypesSplitterState(const QByteArray& state) {
        _store.setValue(Keys::ProductTypesSplitterState, state);
    }
private:
    SettingsManager();

    SettingsStore _store;
    QString _testProfile = "none";
};

