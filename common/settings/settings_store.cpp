#include "settings_store.h"
#include "common/logger/logger.h"

void SettingsStore::init(const QString& path) {
    if (_settings) {
        zError("❌ SettingsStore már inicializálva van, reinit nem engedélyezett!");
        return;
    }

    if (path.isEmpty()) {
        zError("❌ SettingsStore init: üres path nem engedélyezett!");
        return;
    }

    if (!QFile::exists(path)) {
        zError() << "❌ SettingsStore init: a fájl nem létezik: " << path;
        return;
    }

    _settings = std::make_unique<QSettings>(path, QSettings::IniFormat);
    zInfo() << "✅ Settings inicializálva: " << path;
}

// QVariant SettingsStore::value(const QString& key, const QVariant& def) const {
//     auto r = read(key);

//     // Missing + fallback → visszaírjuk a primary-ba (ha nem test mode)
//     if (r.state == ReadResult::State::Missing && !r.value.isEmpty()) {
//         if (!_testMode && _settings) {
//             _settings->setValue(key, r.value);
//             _settings->sync();
//         }
//         return r.value;
//     }

//     // NullValue / EmptyString / NormalValue
//     return r.value.isEmpty() ? def : r.value;
// }
QVariant SettingsStore::value(const QString& key, const QVariant& def) const
{
    ReadResult r = read(key);

    switch (r.state) {

    case ReadResult::State::Missing:
        // Missing + fallback → fallback érték
        if (!r.value.isEmpty()) {
            // fallback érték visszaírása primary-be (sandboxolt tesztmódban is biztonságos)
            if (_settings) {
                _settings->setValue(key, r.value);
                _settings->sync();
            }
            return r.value;
        }
        // Missing + nincs fallback → default
        return def;

    case ReadResult::State::NullValue:
        return def;

    case ReadResult::State::EmptyString:
        // Üres string = szándékos érték
        return "";

    case ReadResult::State::NormalValue:
        return r.value;
    }

    // Biztonsági fallback
    return def;
}



SettingsStore::ReadResult SettingsStore::read(const QString& key) const {
    // 1) Ha nincs primary vagy a fájl fizikailag hiányzik → Missing
    if (!_settings ||
        !QFile::exists(_settings->fileName()) ||   // <-- EZ A HIÁNYZÓ RÉSZ
        !_settings->contains(key)) {

        // 1/a) Ha van fallback és tartalmazza → fallback érték
        if (_fallback && _fallback->contains(key)) {
            QString fb = _fallback->value(key).toString();
            return { ReadResult::State::Missing, fb };
        }

        // 1/b) Fallback sem tartalmazza → Missing + üres
        return { ReadResult::State::Missing, "" };
    }



    // 2) Primary tartalmazza → vizsgáljuk az értéket
    QVariant v = _settings->value(key);

    if (v.isNull()) {
        return { ReadResult::State::NullValue, "" };
    }

    QString s = v.toString();
    if (s.isEmpty()) {
        return { ReadResult::State::EmptyString, "" };
    }

    return { ReadResult::State::NormalValue, s };
}


void SettingsStore::setValue(const QString& key, const QVariant& value) {
    if (_testMode) return;
    if (_settings)
        _settings->setValue(key, value);
}

void SettingsStore::setValue_persistent(const QString& key, const QString& value) {
    setValue(key, value);
    sync();
}

void SettingsStore::setValue_persistent(const QString& key, const QByteArray& value) {
    setValue(key, value);
    sync();
}

void SettingsStore::sync() {
    if (_testMode) return;
    if (_settings) _settings->sync();
}
