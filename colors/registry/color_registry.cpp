#include "colors/registry/color_registry.h"

// --- Lookup API ---

const NamedColor*
ColorRegistry::findByCode(const QString& code) const
{
    return findIf([&](const NamedColor& c){
        return c.code().compare(code, Qt::CaseInsensitive) == 0;
    });
}

QVector<NamedColor>
ColorRegistry::findBySystem(RalSystem sys) const
{
    return findAll([&](const NamedColor& c){
        return c.system() == sys;
    });
}

// #include "colors/registry/color_registry.h"

// // Singleton implementáció
// ColorRegistry& ColorRegistry::instance() {
//     static ColorRegistry reg;
//     return reg;
// }

// void ColorRegistry::clear() {
//     _items.clear();
//     _byCode.clear();
// }

// void ColorRegistry::setData(const QVector<NamedColor>& v) {
//     clear();
//     for (const auto& nc : v)
//         insert(nc);
// }

// void ColorRegistry::insert(const NamedColor& nc) {
//     Base::guardInstanceUsage();
//     int idx = _items.size();
//     _items.append(nc);
//     _byCode.insert(nc.code(), idx);
// }

// // const NamedColor* ColorRegistry::findByCode(const QString& code) const {
// //     auto it = _byCode.find(code);
// //     if (it == _byCode.end())
// //         return nullptr;

// //     return findIf([&](const NamedColor& nc){
// //         return &nc == &_items[it.value()];
// //     });
// // }

// const NamedColor* ColorRegistry::findByCode(const QString& code) const {
//     Base::guardInstanceUsage();
//     auto it = _byCode.find(code);
//     if (it != _byCode.end()) {
//         return &_items[it.value()];
//     }
//     return nullptr;
// }


// // QVector<NamedColor> findBySystem(RalSystem sys) const {
// //     Base::guardInstanceUsage();
// //     QVector<NamedColor> result;
// //     for (const auto& nc : _items)
// //         if (nc.system() == sys)
// //             result.append(nc);
// //     return result;
// // }
// QVector<NamedColor> ColorRegistry::findBySystem(RalSystem sys) const
// {
//     return findAll([&](const NamedColor& nc){
//         return nc.system() == sys;
//     });
// }




