#include "materials/registry/material_registry.h"
#include "common/logger/logger.h"
#include "common/registry/barcode_table.h"
#include "common/logger/event_logger.h"
#include "common/system/verbose_manager.h"


MaterialRegistry &MaterialRegistry::instance() {
    static MaterialRegistry reg;
    return reg;
}

const MaterialMaster* MaterialRegistry::findById(const QUuid& id) const {
    for (const auto& m : _data) if (m.id == id) return &m;
    return nullptr;
}

const MaterialMaster* MaterialRegistry::findByBarcode(const QString& barcode) const {
    for (const auto& m : _data)
        if (m.barcode == barcode)
            return &m;
    return nullptr;
}

bool MaterialRegistry::isBarcodeUnique(const QString& barcode) const {
    for (const auto& m : _data)
        if (m.barcode == barcode)
            return false;
    return true;
}

const IdentifiableEntity* MaterialRegistry::findEntityById(const QUuid& id) const {
    if (auto* mat = findById(id)) {
        return mat; // implicit upcast MaterialMaster* → IdentifiableEntity*
    }
    return nullptr;
}


// bool MaterialRegistry::registerData(const MaterialMaster& material) {
//     if (!isBarcodeUnique(material.barcode))
//         return false;
//     _data.append(material);
//     return true;
// }

//void MaterialRegistry::setData(const QVector<MaterialMaster> &v) { _data = v;}


bool MaterialRegistry::registerData(const MaterialMaster& material) {
    // 🔍 Globális uniqueness check – fail fast, ha ütközés van
    if (!BarcodeTable::instance().checkUnique(material.barcode, typeName(), material.id)) {
        // ⚠️ Audit jelzés – nem kerül be az adat
        zWarning(QString("Material barcode collision: %1 (%2)")
                     .arg(material.barcode, material.name));
        zEventWARN(QString("Material barcode ütközés: %1 (%2)")
                       .arg(material.barcode, material.name));
        return false;
    }

    // ✅ Ha unique, regisztráljuk a BarcodeTable-be és a registrybe
    BarcodeTable::instance().registerNew(material.barcode,
                                         typeName(),
                                         material.id);
    _data.append(material);

    if(IS_VERBOSE_THIS()){
    zInfo(QString("Material registered: %1 [%2] {%3}")
                   .arg(material.name)
                   .arg(material.barcode)
                   .arg(material.id.toString(QUuid::WithoutBraces)));
    }

    return true;
}

