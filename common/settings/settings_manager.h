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
        static inline const auto CurrentTabIndex      = "current_tab_index";

        static inline const auto WindowGeometryPercent     = "window_geometry_percent";
        static inline const auto ScreenSize                = "screen_size";
        static inline const auto MainSplitterPercent       = "main_splitter_percent";
        static inline const auto ProductTypesSplitterPercent = "product_types_splitter_percent";
        static inline const auto LeftVerticalSplitterPercent = "left_vertical_splitter_percent";
        static inline const auto RightVerticalSplitterPercent = "right_vertical_splitter_percent";

        static inline const auto ProductTreeHeaderPercent = "product_tree_header_percent";

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


    // Current tab
    void setCurrentTabIndex(int i) {
        _store.setValue(Keys::CurrentTabIndex, i);
    }

    int currentTabIndex() const {
        return _store.value(Keys::CurrentTabIndex).toInt();
    }

    void setProductTreeHeaderPercent(const QString& s) {
        _store.setValue_persistent(Keys::ProductTreeHeaderPercent, s);
    }
    QString productTreeHeaderPercent() const {
        return _store.value(Keys::ProductTreeHeaderPercent).toString();
    }

    void setWindowGeometryPercent(const QString& s) {
        _store.setValue_persistent(Keys::WindowGeometryPercent, s);
    }
    QString windowGeometryPercent() const {
        return _store.value(Keys::WindowGeometryPercent).toString();
    }

    void setScreenSizeString(const QString& s) {
        _store.setValue_persistent(Keys::ScreenSize, s);
    }
    QString screenSizeString() const {
        return _store.value(Keys::ScreenSize).toString();
    }

    // a mainon lévő splitter
    void setMainSplitterPercent(const QString& s) {
        _store.setValue_persistent(Keys::MainSplitterPercent, s);
    }
    QString mainSplitterPercent() const {
        return _store.value(Keys::MainSplitterPercent).toString();
    }

    void setProductTypesSplitterPercent(const QString& s) {
        _store.setValue_persistent(Keys::ProductTypesSplitterPercent, s);
    }
    QString productTypesSplitterPercent() const {
        return _store.value(Keys::ProductTypesSplitterPercent).toString();
    }

    void setLeftVerticalSplitterPercent(const QString& s) {
        _store.setValue_persistent(Keys::LeftVerticalSplitterPercent, s);
    }
    QString leftVerticalSplitterPercent() const {
        return _store.value(Keys::LeftVerticalSplitterPercent).toString();
    }

    void setRightVerticalSplitterPercent(const QString& s) {
        _store.setValue_persistent(Keys::RightVerticalSplitterPercent, s);
    }
    QString rightVerticalSplitterPercent() const {
        return _store.value(Keys::RightVerticalSplitterPercent).toString();
    }

private:
    SettingsManager();

    SettingsStore _store;
    QString _testProfile = "none";
};

