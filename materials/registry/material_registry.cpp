#include "materials/registry/material_registry.h"
#include "common/registry/barcode_registry_helper.h"
//#include "common/model/entity_format_helper.h"

MaterialRegistry::MaterialRegistry()
    : RegistryEngine<MaterialMaster>("MaterialRegistry", "MaterialMaster")
{}

MaterialRegistry& MaterialRegistry::instance() {
    static MaterialRegistry inst;
    return inst;   // <-- nincs guard
}

void MaterialRegistry::initializeSingleton() {
    auto& inst = instance();   // <-- ugyanaz a példány
    inst.initialize();         // <-- explicit init
}


bool MaterialRegistry::registerData(const MaterialMaster& e) {
    return BarcodeRegistryHelper::insert(*this, e);
}


// bool MaterialRegistry::registerData(const MaterialMaster& e) {
//     if (!BarcodeRegistryHelper::registerEntity(e, typeName()))
//         return false;

//     add(e);

//     if (IS_VERBOSE_THIS()) {
//         QString msg = EntityFormatHelper::fullDebug(e, typeName());
//         zInfo(msg);
//     }

//     return true;
// }

// bool MaterialRegistry::registerData(const MaterialMaster& e) {
//     //guardInstanceUsage();
//     // 1) BarcodeRegistry integráció – domain logika
//     if constexpr (requires { e.barcode; e.id; e.name; }) {
//         auto& barcodeRegistry = BarcodeRegistry::instance();

//         if (!barcodeRegistry.registerNew(e.barcode, typeName(), e.id, e.name)) {
//             return false; // audit már megtörtént
//         }
//     }

//     // 2) CRUD add
//     add(e);

//     // 3) Verbose log (ha van name mező)
//     if constexpr (requires { e.name; e.barcode; e.id; }) {
//         if (IS_VERBOSE_THIS()) {
//             zInfo(QString("Registered: %1 [%2] {%3}")
//                       .arg(e.name)
//                       .arg(e.barcode)
//                       .arg(e.id.toString(QUuid::WithoutBraces)));
//         }
//     }

//     return true;
// }
// MaterialRegistry &MaterialRegistry::instance() {
//     static MaterialRegistry reg;
//     reg.guardInstanceUsage();
//     return reg;
// }

// const MaterialMaster* MaterialRegistry::findById(const QUuid& id) const {
//     for (const auto& m : _data) if (m.id == id) return &m;
//     return nullptr;
// }

// const MaterialMaster* MaterialRegistry::findByBarcode(const QString& barcode) const {
//     for (const auto& m : _data)
//         if (m.barcode == barcode)
//             return &m;
//     return nullptr;
// }

// bool MaterialRegistry::isBarcodeUnique(const QString& barcode) const {
//     for (const auto& m : _data)
//         if (m.barcode == barcode)
//             return false;
//     return true;
// }

// const BarcodeIdentifiableEntity* MaterialRegistry::findEntityById(const QUuid& id) const {
//     if (auto* mat = findById(id)) {
//         return mat; // implicit upcast MaterialMaster* → IdentifiableEntity*
//     }
//     return nullptr;
// }


// // bool MaterialRegistry::registerData(const MaterialMaster& material) {
// //     if (!isBarcodeUnique(material.barcode))
// //         return false;
// //     _data.append(material);
// //     return true;
// // }

// //void MaterialRegistry::setData(const QVector<MaterialMaster> &v) { _data = v;}


// bool MaterialRegistry::registerData(const MaterialMaster& material) {

//     auto& barcodeRegistry = BarcodeRegistry::instance();

//     // ✅ Ha unique, a BarcodeRegistry::registerNew maga ellenőrzi és auditál
//     if (!barcodeRegistry.registerNew(material.barcode, typeName(), material.id, material.name)) {
//         // Audit WARN már a BarcodeRegistry-ben megtörtént
//         return false;
//     }

//     _data.append(material);

//     if(IS_VERBOSE_THIS()){
//     zInfo(QString("Material registered: %1 [%2] {%3}")
//                    .arg(material.name)
//                    .arg(material.barcode)
//                    .arg(material.id.toString(QUuid::WithoutBraces)));
//     }

//     return true;
// }

