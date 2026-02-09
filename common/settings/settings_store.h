#pragma once

#include <QSettings>
#include <QFile>
#include <memory>

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
    struct ReadResult {
        enum class State { Missing, NullValue, EmptyString, NormalValue };
        State state;
        QString value;
    };

    SettingsStore() = default;

    void init(const QString& path);

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    ReadResult read(const QString& key) const;

    void setValue(const QString& key, const QVariant& value);
    void setValue_persistent(const QString& key, const QString& value);
    void setValue_persistent(const QString& key, const QByteArray& value);

    void sync();
    bool isInitialized() const { return _settings != nullptr; }

    void setTestMode(bool b) { _testMode = b; }

    void setFallback(QSettings* fb) { _fallback = fb; }

private:
    std::unique_ptr<QSettings> _settings;
    bool _testMode = false;
    QSettings* _fallback = nullptr;
};
