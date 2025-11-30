#include "namedcolor.h"
#include "common/logger/logger.h"
#include <QMap>
#include <QColor>
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

// Segédfüggvények: extended RAL kód normalizálása
NamedColor::NormalizedRal NamedColor::normalizeRalExtended(const QString& raw) {
    QString code = raw.trimmed().toUpper();

    QRegularExpression classicRe(R"(^(?:RAL[-\s]*)?(\d{4})$)");
    auto match = classicRe.match(code);
    if (match.hasMatch())
        return {"RAL " + match.captured(1), RalSystem::Classic};

    QRegularExpression designRe(R"(^(?:RAL[-\s]*)?(\d{3})[\s-]*(\d{2})[\s-]*(\d{2,3})$)");
    match = designRe.match(code);
    if (match.hasMatch())
        return {QString("RAL %1 %2 %3")
            .arg(match.captured(1))
            .arg(match.captured(2))
            .arg(match.captured(3)),  RalSystem::Design};

    QRegularExpression plasticsRe(R"(^(?:RAL[-\s]*)?(P[12])\s*(?:(\d{3,4})[\s-]*(\d{2})[\s-]*(\d{2,3}))$)");
    match = plasticsRe.match(code);
    if (match.hasMatch()){
        RalSystem sys = (match.captured(1) == "P1") ? RalSystem::Plastic1 : RalSystem::Plastic2;
        return {QString("RAL %1 %2 %3 %4")
            .arg(match.captured(1))
            .arg(match.captured(2))
            .arg(match.captured(3))
            .arg(match.captured(4)), sys};
    }

    return { "", RalSystem::Unknown };
}

// Getterek
QColor NamedColor::color() const { return m_color; }
QString NamedColor::name() const { return m_name; }
QString NamedColor::code() const { return m_code; }
RalSystem NamedColor::system() const { return m_system; }

// Statikus factoryk
// NamedColor NamedColor::fromRal(RalSystem system, const QString& ralCode) {
//     auto a  = normalizeRalExtended(ralCode);
//     const QString normKey = a.key;

//     //const QString key = ralCode.trimmed().toUpper();
//     return _ralColors.value(system).value(normKey, NamedColor(Qt::black, "Ismeretlen RAL", normKey, system));
// }

NamedColor NamedColor::fromRal(const QString& ralCode) {
    auto norm = normalizeRalExtended(ralCode);
    if(!norm.isValid()){
        zWarning() << "Érvénytelen RAL kód:" << ralCode << "(normalizált:" << norm.key << ")";
        return NamedColor(Qt::black, "Érvénytelen RAL", ralCode, RalSystem::Unknown);;
    }

    if (_ralColors[norm.system].contains(norm.key))
        return _ralColors[norm.system][norm.key];

    zWarning() << "Ismeretlen RAL kód:" << ralCode << "(normalizált:" << norm.key << ")";
    return NamedColor(Qt::black, "Ismeretlen RAL", norm.key, RalSystem::Unknown);
}

NamedColor NamedColor::fromHex(const QString& hexCode) {
    QColor c(hexCode.trimmed());
    if (!c.isValid()) {
        zWarning() << "Érvénytelen HEX kód:" << hexCode;
        return NamedColor();
    }
    QString name = QColor::colorNames().contains(hexCode.toLower())
                       ? hexCode.toLower()
                       : QString();
    return NamedColor(c, name);
}

// Registry API
bool NamedColor::containsRalColor(const QString& key) {
    auto norm = normalizeRalExtended(key);
    if(!norm.isValid()) return false;

    const auto sysMapIt = _ralColors.constFind(norm.system);
    if (sysMapIt == _ralColors.constEnd()) return false;
    return sysMapIt->contains(norm.key);
}

void NamedColor::insertRalColor(const NamedColor& value) {
    //auto norm = normalizeRalExtended(key);
    //if(!norm.isValid()) return;

    //_ralColors[norm.system].insert(norm.key, value);
    _ralColors[value.system()].insert(value.code(), value);
}

void NamedColor::clearRalColors() {
    _ralColors.clear();
}

// Egyéb
QString NamedColor::toString() const {
    const QString systemStr = RalSystemUtils::toString(m_system);
    return QString("%1 (%2) - %3").arg(m_code, systemStr, m_name);
}

bool NamedColor::isValid() const {
    return m_color.isValid();
}
