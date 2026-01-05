#pragma once

#include <QVector>
#include <QUuid>
#include <QString>

#include "barcodes/model/barcode_record.h"
#include "common/registry/base/registry_engine.h"
#include "common/registry/feature/register_me.h"
#include "common/registry/workflow/crud_workflow_policy.h"

/**
 * @brief BarcodeRegistry – globális könyvelés a barcode-okhoz.
 *
 * - Egyedi életút-nyilvántartás (introducedAt, retiredAt).
 * - Globális uniqueness: egy kód soha nem adható ki ellentmondásosan.
 * - CSV-ből betölt, runtime-ban módosul, és visszaír CSV-be.
 */
class BarcodeRegistry
    : public RegistryEngine<BarcodeRecord, CrudWorkflowPolicy>,
      public RegisterMe<BarcodeRegistry>
{
    AUTO_REGISTER_REGISTRY
public:
    static BarcodeRegistry& instance() {
        static BarcodeRegistry reg;
        return reg;
    }

    // 🔍 Keresés
    const BarcodeRecord* findByCode(const QString& code) const;
    bool isBarcodeUnique(const QString& code) const;

    // ➕ Új barcode regisztrálása / életút-kezelés
    bool registerNew(const QString& code,
                     const QString& entityType,
                     const QUuid& id,
                     const QString& name);

    bool retire(const QString& code, const QString& reason);

    // 💾 CSV persist (BarcodeRepository-on keresztül)
    void persist() const override;

protected:
    // Workflow hookok
    bool validateDomain(const BarcodeRecord& r) const override;
    bool validateDuplicate(const BarcodeRecord& r) const override;

    bool beforeInsert(const BarcodeRecord& r) override;
    bool beforeUpdate(const BarcodeRecord& r) override;

    void onInsertLog(const BarcodeRecord& r) override;
    void onUpdateLog(const BarcodeRecord& r) override;
    void onRemoveLog(const BarcodeRecord& r) override;

private:
    BarcodeRegistry()
        : RegistryEngine("BarcodeRegistry", "BarcodeRecord")
    {}
};

// #pragma once

// #include <QVector>
// #include <QUuid>
// #include <QString>
// //#include <optional>
// #include "barcodes/model/barcode_record.h"
// #include "common/registry/base/registry_engine_base.h"
// #include "common/registry/feature/register_me.h"

// /**
//  * @brief BarcodeRegistry – globális könyvelés a barcode-okhoz.
//  *
//  * Hunglish magyarázat:
//  * - A MaterialRegistry mintájára épül, de BarcodeRecord-okat kezel.
//  * - Singleton, auditbarát elemszám riportot ad.
//  * - Globális uniqueness: egy kód soha nem adható ki újra.
//  */
// class BarcodeRegistry : public RegistryEngineBase<BarcodeRecord>,
//                         public RegisterMe<BarcodeRegistry>
// {
//     AUTO_REGISTER_REGISTRY(BarcodeRegistry);
// private:
//     BarcodeRegistry() : RegistryEngineBase<BarcodeRecord>("BarcodeRegistry", "Barcode") {}
//     BarcodeRegistry(const BarcodeRegistry&) = delete;

//     //QVector<BarcodeRecord> _data;  ///< 📦 Globális barcode lista

//     //bool registerData_private(const BarcodeRecord &record, bool persistFlag);
// public:
//     // 🔁 Singleton elérés
//     static BarcodeRegistry& instance();

//     bool add(const BarcodeRecord& r);
//     // Ezek a RegistryEngine API-k nem értelmezhetők BarcodeRecord-ra

//     // ➕ Új barcode regisztrálása
//     bool registerData(const BarcodeRecord& record);
//     bool registerData_volatile(const BarcodeRecord &record);

//     // 🔍 Keresés
//     const BarcodeRecord* findByCode(const QString& code) const;

//     // ✅ Uniqueness check
//     bool isBarcodeUnique(const QString& code) const;

//     //bool isEmpty() const { return _items.isEmpty(); }

//     // 🔍 Azonosító alapján keresés (audit kompatibilis)
//     //const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const override;
//     bool registerNew(const QString &code, const QString &entityType, const QUuid &id, const QString& name);
//     bool retire(const QString &code, const QString &reason);

//     void persist() const;   // 🔧 Új: on-the-fly perzisztálás
// };


