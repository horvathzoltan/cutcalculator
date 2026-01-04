#pragma once

#include "ralsystem.h"
#include <QColor>
#include <QMap>
#include <QString>

/**
 * @brief NamedColor – domain modell, tárolás nélkül.
 *
 * A színek tárolását és auditálását a ColorRegistry végzi.
 * A NamedColor csak reprezentációs logikát tartalmaz (HEX/RAL normalizálás, toString).
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
    static NamedColor fromRal(const QString& ralCode);
    static NamedColor fromHex(const QString& hexCode);

    QString toString() const;
    bool isValid() const;

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

    bool operator==(const NamedColor& other) const {
        return m_code == other.m_code
               && m_system == other.m_system;
    }

private:
    QColor m_color;
    QString m_name;
    QString m_code;
    RalSystem m_system = RalSystem::Unknown;
};
