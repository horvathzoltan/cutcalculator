#pragma once

#include <QVector>
#include <QHash>
#include <QString>
#include "common/registry/registry_base.h"
#include "colors/model/named_color.h"
#include "colors/model/ralsystem.h"

/**
 * @brief ColorRegistry – a betöltött színek törzsadata.
 *
 * A IdentifiableRegistryBase-ből öröklődik, így automatikusan regisztrálja magát
 * a RegistryManager-be. Auditbarát elemszám riportot ad.
 *
 * Feladata:
 *  - NamedColor objektumok tárolása
 *  - Keresés kód és rendszer szerint
 *  - Audit riporthoz elemszám biztosítása
 */
class ColorRegistry : public RegistryBase {
private:
    /// Privát konstruktor – singleton
    ColorRegistry() : RegistryBase("ColorRegistry","NamedColor") {}
    ColorRegistry(const ColorRegistry&) = delete;

    QVector<NamedColor> _data;          ///< Tárolt színek listája
    QHash<QString, int> _byCode;        ///< Gyors lookup kód alapján

public:
    /// Singleton elérés
    static ColorRegistry& instance();

    /// RegistryBase kötelező metódusok
    //QString typeName() const override { return "NamedColor"; }
    int size() const override { return _data.size(); }

    /// Adatkezelő metódusok
    void clear();
    void setData(const QVector<NamedColor>& v);
    void insert(const NamedColor& nc);

    /// Lekérdezések
    const QVector<NamedColor>& readAll() const { return _data; }
    const NamedColor* findByCode(const QString& code) const;
    QVector<NamedColor> findBySystem(RalSystem sys) const;

    /// Segédfüggvény: üres-e a registry
    bool isEmpty() const { return _data.isEmpty(); }


};
