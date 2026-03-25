#pragma once

#include <QVector>
#include <QUuid>
#include <QString>

#include "barcodes/model/barcode_record.h"
#include "common/registry/base/registry_engine_base.h"
#include "common/registry/feature/register_me.h"
#include "common/registry/mixins/bulk_load_mixin.h"
#include "common/registry/mixins/test_support_mixin.h"
//#include "common/registry/mixins/crud_mixin.h"

/**
 * @brief BarcodeRegistry – globális életút ledger a barcode-okhoz.
 *
 * - Append-only ledger (introducedAt, retiredAt).
 * - Globális uniqueness: egy kód soha nem adható ki ellentmondásosan.
 * - Minden módosítás a BarcodeValidatoron keresztül történik.
 * - CSV-ből betölt, runtime-ban módosul, és visszaír CSV-be.
 */
class BarcodeRegistry
    : public RegistryEngineBase<BarcodeRecord>,
      public RegisterMe<BarcodeRegistry>,
      public BulkLoadMixin<BarcodeRegistry, BarcodeRecord>,
      public TestSupportMixin<BarcodeRegistry>
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
    void persist() const;

    void onItemsChanged() override
    {
        zInfo("BarcodeRegistry::onItemsChanged");
        RegistryEngineBase<BarcodeRecord>::onItemsChanged();
    }

private:
    // Ledger-specifikus belső hookok
    bool validateDomain(const BarcodeRecord& r) const;
    bool validateDuplicate(const BarcodeRecord& r) const;

    bool beforeInsert(BarcodeRecord& r);
    bool beforeUpdate(BarcodeRecord& r);

    void onInsertLog(const BarcodeRecord& r);
    void onUpdateLog(const BarcodeRecord& r);
    // törölve – ledger modellben nincs remove
    //void onRemoveLog(const BarcodeRecord& r);

private:
    // 🧩 Barát mixinek – hozzáférnek a protected/private storage API-hoz
    friend struct TestSupportMixin<BarcodeRegistry>;
    friend struct BulkLoadMixin<BarcodeRegistry, BarcodeRecord>;

    // ❌ Ezeket TILOS hívni business logikából
    bool add(const BarcodeRecord&) = delete;
    bool removeInternal(const QUuid&) = delete;
    bool removeInternal(const QUuid&, const QUuid&) = delete;

    // ❗ Ezeket NEM töröljük, mert a mixinek használják
    using RegistryEngineBase<BarcodeRecord>::addAll;
    using RegistryEngineBase<BarcodeRecord>::setAll;
    using RegistryEngineBase<BarcodeRecord>::clear;

    // ❗ Az örökölt updateInternal-t elrejtjük, de nem töröljük
    using RegistryEngineBase<BarcodeRecord>::updateInternal;

    // ✔ A saját ledger-műveletek megmaradnak
    bool insertInternal(const BarcodeRecord& r);

private:
    BarcodeRegistry()
        : RegistryEngineBase("BarcodeRegistry", "BarcodeRecord")
    {}
};

