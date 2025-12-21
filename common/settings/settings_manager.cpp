#include "settings_manager.h"
#include "common/utils/filename_helper.h"
#include "common/utils/geometry_helper.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager _instance;
    return _instance;
}

SettingsManager::SettingsManager() {
    QString _readPath  = FileNameHelper::instance().getSettingsFilePath(false);
    QString _writePath = FileNameHelper::instance().getSettingsFilePath(true);

    // B-terv: ha nincs writePath, de van readPath → másoljuk át
    if (!QFile::exists(_writePath)){
        if( QFile::exists(_readPath)) {
            if (!QFile::copy(_readPath, _writePath)) {
                zError("❌ Nem sikerült átmásolni a settings.ini fájlt a testdata-ból a bináris mellé.");
            }
            else {
                zInfo() << "✅ settings.ini átmásolva a testdata-ból a bináris mellé.";
            }
        }
        else
        {
            // egyik sem létezik → de másolás előtt  a forrás a releváns
            zError() << "❌ Nem található a default settings.ini: " << qPrintable(_readPath);
        }
    }

    // késői init
    _store.init(_writePath);

    bool storeInitialized = _store.isInitialized();
    if(!storeInitialized){
        zError("❌ Nem sikerült settingset inicializálni.");
    }
}


void  SettingsManager::detectTestMode(int argc, char* argv[]) {
    for (int i = 1; i < argc - 1; ++i) {
        QString arg = argv[i];
        QString next = argv[i + 1];
        if (arg == "--test") {
            _testProfile = next.toLower();
            return;
        }
    }

    // ini-ből
    if (_store.value("testmode", false).toBool()) {
        QString testProfile = _store.value("test").toString();
        if (!testProfile.isEmpty()) {
            _testProfile = testProfile.toLower();
        }
    } else{
        _testProfile = "";
    }
    return;
}

/*setValue_persistent*/

void SettingsStore::setValue_persistent(const QString &key, const QString &value) {
    setValue(key, value);
    sync();
}

void SettingsStore::setValue_persistent(const QString& key, const QByteArray& value) {
    setValue(key, value);
    sync();
}




