#include "settings_manager.h"
#include "common/utils/filename_helper.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager _instance;
    return _instance;
}

SettingsManager::SettingsManager() {
//     QString primary =
//         FileNameHelper::instance().pathFor(FileKind::SettingsIni, FileAccess::Write);

//     QString fallback =
//         FileNameHelper::instance().pathFor(FileKind::SettingsIni, FileAccess::Read);



//     // 3) Primary settings.ini inicializálása
//     _store.init(primary);

//     if (!_store.isInitialized()) {
//         zError("❌ Nem sikerült settingset inicializálni.");
//     }

//     _store.setFallback(_fallback.get());
}

void SettingsManager::detectTestMode(int argc, char* argv[])
{
    detectTestMode_private(argc, argv);

    // 1) primary + fallback path
    QString primary =
        FileNameHelper::instance().pathFor(FileKind::SettingsIni, FileAccess::Write);
    QString fallbackPath =
        FileNameHelper::instance().pathFor(FileKind::SettingsIni, FileAccess::Read);

    // 2) fallback ini megnyitása (ha van)
    if (QFile::exists(fallbackPath)) {
        _fallback = std::make_unique<QSettings>(fallbackPath, QSettings::IniFormat);
        zInfo() << "ℹ️ Fallback settings.ini megnyitva:" << fallbackPath;
    } else {
        zWarning() << "⚠️ Fallback settings.ini nem található:" << fallbackPath;
    }

    // 3) ha nincs primary, de van fallback → másoljuk
    if (!QFile::exists(primary) && _fallback) {
        if (QFile::copy(fallbackPath, primary)) {
            zInfo() << "✅ Primary settings.ini létrehozva fallbackből:" << primary;
        } else {
            zError() << "❌ Nem sikerült fallbackből létrehozni a primary settings.ini-t";
        }
    }

    // 4) store init – CSAK MOST
    if (!_store.isInitialized()) {
        _store.init(primary);
        if (!_store.isInitialized()) {
            zError("❌ Nem sikerült settingset inicializálni.");
        }
    }

    // 5) teszt mód, fallback, datapath, stb.
    _store.setTestMode(isTestMode());
    _store.setFallback(_fallback.get());

    QString dp = dataRootPath();
    if (dp.isEmpty()) {
        dp = QDir::home().filePath("CutData");
        QDir().mkpath(dp);
    }

    FileNameHelper::instance().setDataRootPath(dp);

    if (_store.isInitialized() && _store.value("datapath").toString().isEmpty()) {
        _store.setValue_persistent("datapath", dp);
    }
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





