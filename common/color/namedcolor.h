#pragma once

#include "common/color/ralsystem.h"
#include <QColor>
#include <QMap>

// 🌐 NamedColor modell
class NamedColor {
public:
    NamedColor() = default;
    NamedColor(const QColor& color, const QString& name);
    NamedColor(const QString& code); // HEX vagy RAL
    NamedColor(const QColor& color, const QString& name, const QString& code, RalSystem system);

    // 🔍 Lekérdezések
    QColor color() const;
    QString name() const;
    QString code() const;
    RalSystem system() const;

    // 🧩 Statikus segédfüggvények
    static NamedColor fromRal(RalSystem system, const QString& ralCode);
    static NamedColor fromRal(const QString& ralCode);
    static NamedColor fromHex(const QString& hexCode);

    QString toString() const;
    bool isValid() const;

    static void insertRalColor(RalSystem system, const QString& key, const NamedColor& value) {
        _ralColors[system].insert(key, value);
    }
    static void clearRalColors() {
        _ralColors.clear();
    }

private:
    QColor m_color;
    QString m_name;
    QString m_code;
    RalSystem m_system = RalSystem::Unknown;

    // Globális RAL adatbázis (feltöltést külön modul intézi)
    static QMap<RalSystem, QMap<QString, NamedColor>> _ralColors;

    static QString normalizeRalExtended(const QString& raw);
};
