#pragma once

#include "common/color/ralsystem.h"
#include <QColor>
#include <QMap>
#include <QString>

/**
 * @brief Domain modell: névvel ellátott szín (HEX/RAL), RAL rendszerrel.
 *
 * Statikus registry-ben tároljuk a betöltött RAL színkészletet.
 */
class NamedColor {
public:
    NamedColor() = default;
    NamedColor(const QColor& color, const QString& name);
    NamedColor(const QString& code); // HEX vagy RAL (heurisztika)
    NamedColor(const QColor& color, const QString& name, const QString& code, RalSystem system);

    // Lekérdezések
    QColor color() const;
    QString name() const;
    QString code() const;
    RalSystem system() const;

    // Statikus segédfüggvények (RAL/HEX gyárak)
   // static NamedColor fromRal(RalSystem system, const QString& ralCode);
    static NamedColor fromRal(const QString& ralCode);
    static NamedColor fromHex(const QString& hexCode);

    QString toString() const;
    bool isValid() const;

    // Registry API (Assemble fázishoz)
    static bool containsRalColor(const QString& key);
    static void insertRalColor(const NamedColor& value);
    static void clearRalColors();

    struct NormalizedRal {
        QString key;       // pl. "RAL 7016"
        RalSystem system;  // pl. RalSystem::Classic

    public:
        bool isValid(){
            if(key.isEmpty()) return false;
            if(system == RalSystem::Unknown) return false;
            return true;
        }
    };

    static NormalizedRal normalizeRalExtended(const QString& raw);
private:
    QColor m_color;
    QString m_name;
    QString m_code;
    RalSystem m_system = RalSystem::Unknown;

    static QMap<RalSystem, QMap<QString, NamedColor>> _ralColors;


};
