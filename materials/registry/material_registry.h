#pragma once

#include <QVector>
#include <QUuid>

//#include "common/registry/base/registry_engine_base.h"
#include "common/logger/logger.h"
#include "common/registry/base/registry_engine.h"

#include "common/registry/feature/register_me.h"
#include "materials/model/material_master.h"
//#include "common/registry/barcode/barcode_lookup_mixin.h"
#include "common/registry/barcode/barcode_index_mixin.h"
#include "common/registry/workflow/crud_workflow_policy.h"


class MaterialRegistry
    : public RegistryEngine<MaterialMaster, CrudWorkflowPolicy>,
      public BarcodeIndexMixin<MaterialRegistry, MaterialMaster>,
      public RegisterMe<MaterialRegistry>
{
    AUTO_REGISTER_REGISTRY
public:
    static MaterialRegistry& instance() {
        static MaterialRegistry inst;
        return inst;
    }

    // Lookup API
    QVector<MaterialMaster> findByType(MaterialType type) const;
    QVector<MaterialMaster> findByShape(CrossSectionShape::Shape shape) const;
    QVector<MaterialMaster> findByColor(const NamedColor& color) const;

    void persist() const override {};

protected:
    // Domain hookok
    bool validateDomain(const MaterialMaster& m) const override;
    bool validateDuplicate(const MaterialMaster& m) const override;

    bool beforeInsert(const MaterialMaster& m) override;
    bool beforeUpdate(const MaterialMaster& m) override;

    void onInsertLog(const MaterialMaster& m) override;
    void onUpdateLog(const MaterialMaster& m) override;
    void onRemoveLog(const MaterialMaster& m) override;

private:
    MaterialRegistry()
        : RegistryEngine< MaterialMaster, CrudWorkflowPolicy>(
              L("MaterialRegistry"), L("MaterialMaster"))
    {}

};


// #pragma once

// #include <QVector>
// #include <QUuid>
// #include <QString>
// #include "common/registry/barcode/barcode_identifiable_registry_engine.h"
// #include "common/system/verbose_manager.h"
// #include "materials/model/material_master.h"
// #include "common/registry/feature/register_me.h"

// class MaterialRegistry : public BarcodeIdentifiableRegistryEngine<MaterialMaster>,
//                          public RegisterMe<MaterialRegistry>
// {
//     AUTO_REGISTER_REGISTRY(MaterialRegistry);
// public:
//     static MaterialRegistry& instance() {
//         static MaterialRegistry inst;
//         return inst;
//     }

// private:
//     MaterialRegistry(); // <-- saját ctor

// public:
//     bool insert(const MaterialMaster& e);
//     bool verbose() const { return IS_VERBOSE_THIS(); }
// };
