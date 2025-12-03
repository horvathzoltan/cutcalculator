#include "colors/registry/color_registry.h"

// Singleton implementáció
ColorRegistry& ColorRegistry::instance() {
    static ColorRegistry reg;
    return reg;
}

void ColorRegistry::clear() {
    _data.clear();
    _byCode.clear();
}

void ColorRegistry::setData(const QVector<NamedColor>& v) {
    clear();
    _data = v;
    for (int i = 0; i < _data.size(); ++i) {
        _byCode.insert(_data[i].code(), i);
    }
}

void ColorRegistry::insert(const NamedColor& nc) {
    int idx = _data.size();
    _data.append(nc);
    _byCode.insert(nc.code(), idx);
}

const NamedColor* ColorRegistry::findByCode(const QString& code) const {
    auto it = _byCode.find(code);
    if (it != _byCode.end()) {
        return &_data[it.value()];
    }
    return nullptr;
}

QVector<NamedColor> ColorRegistry::findBySystem(RalSystem sys) const {
    QVector<NamedColor> result;
    for (const auto& nc : _data) {
        if (nc.system() == sys) {
            result.append(nc);
        }
    }
    return result;
}
