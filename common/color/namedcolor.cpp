#include "namedcolor.h"
#include <QMap>
#include <QColor>
#include <QDebug>
#include <QRegularExpression>

QMap<RalSystem, QMap<QString, NamedColor>> NamedColor::_ralColors;

// Konstruktorok
NamedColor::NamedColor(const QColor& color, const QString& name)
    : m_color(color), m_name(name), m_code(color.name().toUpper()) {}

NamedColor::NamedColor(const QColor& color, const QString& name, const QString& code, RalSystem system)
    : m_color(color), m_name(name), m_code(code), m_system(system) {}

NamedColor::NamedColor(const QString& code) {
    if (code.startsWith("RAL", Qt::CaseInsensitive)) {
        *this = fromRal(code);
    } else if (code.startsWith('#')) {
        QColor c(code);
        if (c.isValid()) {
            m_color = c;
            m_name = code.toUpper();
            m_code = code.toUpper();
        } else {
            m_color = QColor(Qt::black);
            m_name = "Invalid HEX";
            m_code = code;
        }
        *this = fromHex(code);
    } else if (QColor::colorNames().contains(code.toLower())) {
        m_color = QColor(code.toLower());
        m_name = code.toLower();
        m_code = m_color.name().toUpper();
    } else {
        m_color = QColor(Qt::black);
        m_name = "Invalid HEX";
        m_code = code;
    }
}

// Segédfüggvények
QString NamedColor::normalizeRalExtended(const QString& raw) {
    QString code = raw.trimmed().toUpper();

    QRegularExpression classicRe("^RAL[-\\s]*(\\d{4})$");
    auto match = classicRe.match(code);
    if (match.hasMatch())
        return "RAL " + match.captured(1);

    QRegularExpression designRe("^RAL[-\\s]*(\\d{3})[-\\s]*(\\d{2})[-\\s]*(\\d{2})$");
    match = designRe.match(code);
    if (match.hasMatch())
        return QString("RAL %1 %2 %3")
            .arg(match.captured(1))
            .arg(match.captured(2))
            .arg(match.captured(3));

    QRegularExpression plasticsRe("^RAL[-\\s]*P1[-\\s]*(\\d{3})[-\\s]*(\\d{2})[-\\s]*(\\d{2})$");
    match = plasticsRe.match(code);
    if (match.hasMatch())
        return QString("RAL P1 %1 %2 %3")
            .arg(match.captured(1))
            .arg(match.captured(2))
            .arg(match.captured(3));

    return code;
}

// Getterek
QColor NamedColor::color() const { return m_color; }
QString NamedColor::name() const { return m_name; }
QString NamedColor::code() const { return m_code; }
RalSystem NamedColor::system() const { return m_system; }

// Statikus factoryk
NamedColor NamedColor::fromRal(RalSystem system, const QString& ralCode) {
    const QString key = ralCode.trimmed().toUpper();
    return _ralColors.value(system).value(key, NamedColor(Qt::black, "Ismeretlen RAL", key, system));
}

NamedColor NamedColor::fromRal(const QString& ralCode) {
    const QString key = normalizeRalExtended(ralCode);
    for (auto it = _ralColors.constBegin(); it != _ralColors.constEnd(); ++it) {
        const auto& systemMap = it.value();
        if (systemMap.contains(key))
            return systemMap.value(key);
    }
    qWarning() << "Ismeretlen RAL kód (globális keresés):" << ralCode;
    return NamedColor(Qt::black, "Ismeretlen RAL", key, RalSystem::Unknown);
}

NamedColor NamedColor::fromHex(const QString& hexCode) {
    QColor c(hexCode.trimmed());
    if (!c.isValid()) {
        qWarning() << "Érvénytelen HEX kód:" << hexCode;
        return NamedColor();
    }
    QString name = QColor::colorNames().contains(hexCode.toLower())
                       ? hexCode.toLower()
                       : QString();
    return NamedColor(c, name);
}

// Egyéb
QString NamedColor::toString() const {
    QString systemStr = RalSystemUtils::toString(m_system);
    return QString("%1 (%2) - %3").arg(m_code, systemStr, m_name);
}

bool NamedColor::isValid() const {
    return m_color.isValid();
}
