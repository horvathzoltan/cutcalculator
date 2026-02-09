#include "settings_manager.h"
#include "common/utils/filename_helper.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager _instance;
    return _instance;
}

SettingsManager::SettingsManager() {
    QString _readPath  =
        FileNameHelper::instance().pathFor(FileKind::SettingsIni, FileAccess::Read);

    QString _writePath =
        FileNameHelper::instance().pathFor(FileKind::SettingsIni, FileAccess::Write);

    // 1) Fallback settings.ini megnyitása (csak olvasásra)
    if (QFile::exists(_readPath)) {
        _fallback = std::make_unique<QSettings>(_readPath, QSettings::IniFormat);
        zInfo() << "ℹ️ Fallback settings.ini megnyitva:" << _readPath;
    } else {
        zWarning() << "⚠️ Fallback settings.ini nem található:" << _readPath;
    }

    // 2) Ha nincs primary settings.ini → másoljuk át a fallbacket
    if (!QFile::exists(_writePath)) {
        if (QFile::exists(_readPath)) {
            if (!QFile::copy(_readPath, _writePath)) {
                zError("❌ Nem sikerült átmásolni a settings.ini fájlt a testdata-ból a bináris mellé.");
            } else {
                zInfo() << "✅ settings.ini átmásolva a testdata-ból a bináris mellé.";
            }
        } else {
            zError() << "❌ Nem található a default settings.ini: " << _readPath;
        }
    }

    // 3) Primary settings.ini inicializálása
    _store.init(_writePath);

    if (!_store.isInitialized()) {
        zError("❌ Nem sikerült settingset inicializálni.");
    }
}

void SettingsManager::detectTestMode(int argc, char* argv[]) {
    detectTestMode_private(argc, argv);

    // ⭐ Itt már biztosan tudjuk, hogy test mode van-e
    _store.setTestMode(isTestMode());
    _store.setFallback(_fallback.get());
}

void  SettingsManager::detectTestMode_private(int argc, char* argv[]) {
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





