#pragma once

#include <QVector>
#include <QUuid>
#include <QString>
#include "common/registry/barcode/barcode_identifiable_registry_engine.h"
#include "common/system/verbose_manager.h"
#include "materials/model/material_master.h"
#include "common/registry/feature/register_me.h"

class MaterialRegistry : public BarcodeIdentifiableRegistryEngine<MaterialMaster>,
                         public RegisterMe<MaterialRegistry>
{
    AUTO_REGISTER_REGISTRY(MaterialRegistry);
public:
    static MaterialRegistry& instance() {
        static MaterialRegistry inst;
        return inst;
    }

private:
    MaterialRegistry(); // <-- saját ctor

public:
    bool insert(const MaterialMaster& e);
    bool verbose() const { return IS_VERBOSE_THIS(); }
};
