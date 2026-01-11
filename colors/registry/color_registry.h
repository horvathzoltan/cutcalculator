#pragma once

#include <QVector>
#include <QString>

#include "common/registry/base/registry_engine_base.h"
#include "colors/model/named_color.h"
#include "common/registry/feature/register_me.h"

class ColorRegistry
    : public RegistryEngineBase<NamedColor>,
    public RegisterMe<ColorRegistry>
{
    AUTO_REGISTER_REGISTRY
public:
    static ColorRegistry& instance() {
        static ColorRegistry inst;
        return inst;
    }

    // Lookup API
    const NamedColor* findByCode(const QString& code) const;
    QVector<NamedColor> findBySystem(RalSystem sys) const;

private:
    ColorRegistry()
        : RegistryEngineBase("ColorRegistry", "NamedColor")
    {}
};


// #pragma once

// #include <QVector>
// #include <QHash>
// #include <QString>
// #include "colors/model/named_color.h"
// #include "colors/model/ralsystem.h"
// #include "common/registry/base/registry_engine_base.h"
// #include "common/registry/feature/register_me.h"

// /**
//  * @brief ColorRegistry – a betöltött színek törzsadata.
//  *
//  * A IdentifiableRegistryBase-ből öröklődik, így automatikusan regisztrálja magát
//  * a RegistryManager-be. Auditbarát elemszám riportot ad.
//  *
//  * Feladata:
//  *  - NamedColor objektumok tárolása
//  *  - Keresés kód és rendszer szerint
//  *  - Audit riporthoz elemszám biztosítása
//  */
// class ColorRegistry : public RegistryEngineBase<NamedColor>,
//                       public RegisterMe<ColorRegistry>
// {
//     AUTO_REGISTER_REGISTRY(ColorRegistry);
// private:
//     /// Privát konstruktor – singleton
//     ColorRegistry() : RegistryEngineBase<NamedColor>("ColorRegistry","NamedColor") {}
//     ColorRegistry(const ColorRegistry&) = delete;

//     //QVector<NamedColor> _data;          ///< Tárolt színek listája
//     QHash<QString, int> _byCode;        ///< Gyors lookup kód alapján

// public:
//     /// Singleton elérés
//     static ColorRegistry& instance();

//     /// Adatkezelő metódusok
//     void clear();
//     void setData(const QVector<NamedColor>& v);
//     void insert(const NamedColor& nc);

//     /// Lekérdezések
//     //const QVector<NamedColor>& readAll() const { return _data; }
//     const NamedColor* findByCode(const QString& code) const;
//     QVector<NamedColor> findBySystem(RalSystem sys) const;

// };
