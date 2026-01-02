#pragma once

#include <QVector>
#include <QUuid>
#include <QString>
//#include <optional>
#include "barcodes/model/barcode_record.h"
//#include "common/registry/barcode_identifiable_registry_base.h"
//#include "common/model/barcode_identifiable_entity.h"
#include "common/registry/base/registry_engine_base.h"
//#include "common/registry/registry_base.h"

/**
 * @brief BarcodeRegistry – globális könyvelés a barcode-okhoz.
 *
 * Hunglish magyarázat:
 * - A MaterialRegistry mintájára épül, de BarcodeRecord-okat kezel.
 * - Singleton, auditbarát elemszám riportot ad.
 * - Globális uniqueness: egy kód soha nem adható ki újra.
 */
class BarcodeRegistry : public RegistryEngineBase<BarcodeRecord>,
                        public RegisterMe<BarcodeRegistry>
{
    AUTO_REGISTER_REGISTRY(BarcodeRegistry);
private:
    BarcodeRegistry() : RegistryEngineBase<BarcodeRecord>("BarcodeRegistry", "Barcode") {}
    BarcodeRegistry(const BarcodeRegistry&) = delete;

    //QVector<BarcodeRecord> _data;  ///< 📦 Globális barcode lista

    //bool registerData_private(const BarcodeRecord &record, bool persistFlag);
public:
    // 🔁 Singleton elérés
    static BarcodeRegistry& instance();

    bool add(const BarcodeRecord& r);
    // Ezek a RegistryEngine API-k nem értelmezhetők BarcodeRecord-ra

    // ➕ Új barcode regisztrálása
    bool registerData(const BarcodeRecord& record);

    // 🔍 Keresés
    const BarcodeRecord* findByCode(const QString& code) const;

    // ✅ Uniqueness check
    bool isBarcodeUnique(const QString& code) const;

    //bool isEmpty() const { return _items.isEmpty(); }

    // 🔍 Azonosító alapján keresés (audit kompatibilis)
    //const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const override;
    bool registerNew(const QString &code, const QString &entityType, const QUuid &id, const QString& name);
    bool retire(const QString &code, const QString &reason);

    void persist() const;   // 🔧 Új: on-the-fly perzisztálás
    bool registerData_volatile(const BarcodeRecord &record);
};


