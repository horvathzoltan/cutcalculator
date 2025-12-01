#pragma once

#include <QVector>
#include <QUuid>
#include <QString>
//#include <optional>
#include "materials/model/material_master.h"
#include "common/registry/registry_base.h"

/**
 * @brief MaterialRegistry – a betöltött anyagok törzsadata.
 *
 * A RegistryBase-ből öröklődik, így automatikusan regisztrálja magát
 * a RegistryManager-be. Auditbarát elemszám riportot ad.
 */

class MaterialRegistry: public RegistryBase {
private:
    MaterialRegistry() : RegistryBase("MaterialRegistry") {}//    MaterialRegistry() = default;  // 🔐 Privát konstruktor a singletonhoz
    MaterialRegistry(const MaterialRegistry&) = delete;

    QVector<MaterialMaster> _data;  // 📦 Betöltött anyagtörzs lista
public:

    // 🔁 Singleton elérés
    static MaterialRegistry& instance();

    QString typeName() const override { return "MaterialMaster"; }
    int count() const override { return _data.size(); }

    void setData(const QVector<MaterialMaster>& v) { _data = v;}
    // ➕ Új anyag hozzáadása, csak ha code egyedi
    bool registerData(const MaterialMaster& material);

    const QVector<MaterialMaster>& readAll() const { return _data;}
    const MaterialMaster* findById(const QUuid& id) const;
    const MaterialMaster* findByBarcode(const QString& barcode) const;

    bool isBarcodeUnique(const QString& barcode) const;

    bool isEmpty() const { return _data.isEmpty(); }

    int size() const { return _data.size(); }
};
