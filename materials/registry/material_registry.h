#pragma once

#include <QVector>
#include <QUuid>
#include <QString>
#include "common/registry/barcode/barcode_identifiable_registry_engine.h"
#include "common/system/verbose_manager.h"
#include "materials/model/material_master.h"

class MaterialRegistry : public BarcodeIdentifiableRegistryEngine<MaterialMaster>,
                         public RegisterMe<MaterialRegistry>
{
    AUTO_REGISTER_REGISTRY(MaterialRegistry);
public:
    static MaterialRegistry& instance();
    //static void initializeSingleton();

private:
    MaterialRegistry(); // <-- saját ctor
    //friend void initializeAllRegistries();

public:

    bool registerData(const MaterialMaster& e);

    bool verbose() const { return IS_VERBOSE_THIS(); }
};
