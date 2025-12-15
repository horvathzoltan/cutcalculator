#pragma once

#include <QVector>
#include <QUuid>
#include <QString>
#include <optional>
#include "barcodes/model/barcode_record.h"
#include "common/registry/registry_base.h"

/**
 * @brief BarcodeRegistry – globális könyvelés a barcode-okhoz.
 *
 * Hunglish magyarázat:
 * - A MaterialRegistry mintájára épül, de BarcodeRecord-okat kezel.
 * - Singleton, auditbarát elemszám riportot ad.
 * - Globális uniqueness: egy kód soha nem adható ki újra.
 */
class BarcodeRegistry : public IdentifiableRegistryBase {
private:
    BarcodeRegistry() : IdentifiableRegistryBase("BarcodeRegistry", "Barcode") {}
    BarcodeRegistry(const BarcodeRegistry&) = delete;

    QVector<BarcodeRecord> _data;  ///< 📦 Globális barcode lista

    bool registerData_private(const BarcodeRecord &record, bool persistFlag);
public:
    // 🔁 Singleton elérés
    static BarcodeRegistry& instance();

    // 📊 Méret riport
    int size() const override { return _data.size(); }

    // ➕ Új barcode regisztrálása
    bool registerData(const BarcodeRecord& record);

    // 📖 Olvasás
    const QVector<BarcodeRecord>& readAll() const { return _data; }

    // 🔍 Keresés
    const BarcodeRecord* findByCode(const QString& code) const;

    // ✅ Uniqueness check
    bool isBarcodeUnique(const QString& code) const;

    bool isEmpty() const { return _data.isEmpty(); }

    // 🔍 Azonosító alapján keresés (audit kompatibilis)
    const IdentifiableEntity* findEntityById(const QUuid& id) const override;
    bool registerNew(const QString &code, const QString &entityType, const QUuid &id);
    bool retire(const QString &code, const QString &reason);

    void persist() const;   // 🔧 Új: on-the-fly perzisztálás
    bool registerData_volatile(const BarcodeRecord &record);
};
