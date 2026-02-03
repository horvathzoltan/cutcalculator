#pragma once

#include "common/registry/barcode/id_lookup_registry_interface.h"
#include "common/registry/base/barcode_capable_interface.h"
#include "common/registry/base/registry_engine_base.h"
#include "common/registry/mixins/bulk_load_mixin.h"
#include "common/registry/mixins/id_lookup_mixin.h"
#include "common/registry/barcode/barcode_index_mixin.h"
#include "common/registry/feature/register_me.h"
#include "common/registry/mixins/test_support_mixin.h"
#include "materials/model/material_master.h"
#include "common/logger/logger.h"
#include "common/registry/contract_checks.h"

class MaterialRegistry
    : public RegistryEngineBase<MaterialMaster>,
      public IdLookupMixin<MaterialRegistry, MaterialMaster>,
      public IdLookupRegistryInterface,
      public BarcodeIndexMixin<MaterialRegistry, MaterialMaster>,
      public IBarcodeCapable,
      public RegisterMe<MaterialRegistry>,
      public TestSupportMixin<MaterialRegistry>,
      public BulkLoadMixin<MaterialRegistry, MaterialMaster>,
{
    AUTO_REGISTER_REGISTRY

public:
    static MaterialRegistry& instance() {
        static MaterialRegistry inst;
        return inst;
    }

    const IdentifiableEntity* findEntityById(const QUuid& id) const override {
        if (auto* e = findById(id))
            return static_cast<const IdentifiableEntity*>(e);
        return nullptr;
    }

    // Lookup API
    QVector<MaterialMaster> readAll() const { return RegistryEngineBase<MaterialMaster>::readAll(); }
    const MaterialMaster* findById(const MaterialMaster::IdType& id) const { return RegistryEngineBase<MaterialMaster>::findById(id); }


    void onItemsChanged() override
    {
        zInfo("róka4: MaterialRegistry::onItemsChanged fut");
        RegistryEngineBase<MaterialMaster>::onItemsChanged();
    }

private:
    MaterialRegistry()
        : RegistryEngineBase<MaterialMaster>(QStringLiteral("MaterialRegistry"),
                                             QStringLiteral("MaterialMaster"))
    {}
    ~MaterialRegistry() = default;
};
REGISTER_LOOKUP_CHECK(MaterialRegistry, MaterialMaster)
