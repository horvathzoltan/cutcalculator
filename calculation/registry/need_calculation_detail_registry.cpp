#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/manager/registry_manager.h"
#include "calculation/repository/need_calculation_detail_repository.h"
//#include "materials/registry/material_registry.h"

// --- Segédek ---

bool NeedCalculationDetailRegistry::isFormulaValid(const QString& f) {
    const auto trimmed = f.trimmed();
    if (trimmed.isEmpty())
        return false;

    if (trimmed.startsWith("w-") || trimmed.startsWith("h-")) {
        bool ok = false;
        trimmed.mid(2).toInt(&ok);
        return ok;
    }

    if (trimmed.startsWith("fixed:")) {
        bool ok = false;
        trimmed.mid(QStringLiteral("fixed:").size()).toInt(&ok);
        return ok;
    }

    return false;
}

bool NeedCalculationDetailRegistry::materialExists(const QUuid& materialId) {
    auto* matBase = RegistryManager::instance().findByTypeName("MaterialMaster");
    if (!matBase)
        return false;

    return matBase->findEntityById(materialId) != nullptr;
}

// --- Kényelmi API ---

bool NeedCalculationDetailRegistry::updateFormula(const QUuid& id,
                                                  const QString& newFormula)
{
    const auto* existing = findById(id);
    if (!existing)
        return false;

    auto updated = *existing;
    updated.formula = newFormula;

    return update(updated);
}


// --- Lookup API ---

QVector<NeedCalculationDetail>
NeedCalculationDetailRegistry::findByCalculation(const QUuid& calcId) const
{
    return findAll([&](const NeedCalculationDetail& d){
        return d.needCalculationId == calcId;
    });
}

// --- Domain hookok ---

bool NeedCalculationDetailRegistry::validateDomain(const NeedCalculationDetail& d) const
{
    return isFormulaValid(d.formula)
    && !d.materialId.isNull()
        && !d.needCalculationId.isNull();
}

bool NeedCalculationDetailRegistry::validateDuplicate(const NeedCalculationDetail& d) const
{
    return !existsBy([&](const NeedCalculationDetail& x){
        return x.needCalculationId == d.needCalculationId
               && x.materialId == d.materialId
               && x.formula == d.formula;
    });
}

bool NeedCalculationDetailRegistry::beforeInsert(const NeedCalculationDetail& d)
{
    return materialExists(d.materialId);
}

bool NeedCalculationDetailRegistry::beforeUpdate(const NeedCalculationDetail& d)
{
    return beforeInsert(d);
}

// --- Log hookok ---

void NeedCalculationDetailRegistry::onInsertLog(const NeedCalculationDetail& d)
{
    zInfo(QString("➕ NeedCalculationDetail INSERT: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

void NeedCalculationDetailRegistry::onUpdateLog(const NeedCalculationDetail& d)
{
    zInfo(QString("✏️ NeedCalculationDetail UPDATE: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

void NeedCalculationDetailRegistry::onRemoveLog(const NeedCalculationDetail& d)
{
    zInfo(QString("🗑️ NeedCalculationDetail REMOVE: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

// --- Persist ---

void NeedCalculationDetailRegistry::persist() const
{
    NeedCalculationDetailRepository::save(readAll());
}

// #include "calculation/registry/need_calculation_detail_registry.h"

// #include "calculation/repository/need_calculation_detail_repository.h"
// #include "materials/registry/material_registry.h"
// #include "common/logger/logger.h"
// #include "common/registry/manager/registry_manager.h"
// #include "common/registry/base/identifiable_registry_helper.h"

// // --- Konstruktor ---

// NeedCalculationDetailRegistry::NeedCalculationDetailRegistry()
//     : IdentifiableRegistryEngine<NeedCalculationDetail>("NeedCalculationDetailRegistry",
//                                                         "NeedCalculationDetail")
// {
//     RegistryManager::instance().registerRepo(this);
// }

// // --- Statikus validátorok ---

// bool NeedCalculationDetailRegistry::isFormulaValid(const QString& f) {
//     const auto trimmed = f.trimmed();
//     if (trimmed.isEmpty())
//         return false;

//     if (trimmed.startsWith("w-") || trimmed.startsWith("h-")) {
//         bool ok = false;
//         trimmed.mid(2).toInt(&ok);
//         return ok;
//     }

//     if (trimmed.startsWith("fixed:")) {
//         bool ok = false;
//         trimmed.mid(QStringLiteral("fixed:").size()).toInt(&ok);
//         return ok;
//     }

//     return false;
// }

// bool NeedCalculationDetailRegistry::materialExists(const QUuid& materialId) {
//     // Audit-barát, registry-manager alapú lookup
//     auto* matBase = RegistryManager::instance().findByTypeName("MaterialMaster");
//     if (!matBase)
//         return false;

//     // Itt feltételezzük, hogy MaterialRegistry IdentifiableRegistryEngine alapú
//     return matBase->findEntityById(materialId) != nullptr;
// }

// // --- CRUD API (workflow-hívók) ---

// bool NeedCalculationDetailRegistry::insert(const NeedCalculationDetail& det) {
//     return IdentifiableRegistryHelper::insert(*this, det);
// }

// bool NeedCalculationDetailRegistry::update(const NeedCalculationDetail& det) {
//     return IdentifiableRegistryHelper::update(*this, det);
// }

// bool NeedCalculationDetailRegistry::remove(const QUuid& id) {
//     return IdentifiableRegistryHelper::remove(*this, id);
// }

// // Csak a formula mezőt módosítjuk → update workflow
// bool NeedCalculationDetailRegistry::updateFormula(const QUuid& id,
//                                                   const QString& newFormula)
// {
//     const auto* existing = findById(id);
//     if (!existing)
//         return false;

//     NeedCalculationDetail updated = *existing;
//     updated.formula = newFormula;

//     return update(updated);
// }

// // --- Lookup API ---

// QVector<NeedCalculationDetail>
// NeedCalculationDetailRegistry::findByCalculation(const QUuid& calcId) const
// {
//     return findAll([&](const NeedCalculationDetail& d){
//         return d.needCalculationId == calcId;
//     });
// }

// // --- Persist hook ---

// void NeedCalculationDetailRegistry::persist() const {
//     NeedCalculationDetailRepository::save(_items);
// }

// // --- Hookok: domain validáció + log ---

// bool NeedCalculationDetailRegistry::beforeInsert(const NeedCalculationDetail& d) {
//     if (!isFormulaValid(d.formula)) {
//         zWarning(QString("⚠️ NeedCalculationDetail insert: invalid formula: %1")
//                      .arg(d.formula));
//         return false;
//     }

//     if (!materialExists(d.materialId)) {
//         zWarning(QString("⚠️ NeedCalculationDetail insert: material not found: %1")
//                      .arg(d.materialId.toString()));
//         return false;
//     }

//     return true;
// }

// bool NeedCalculationDetailRegistry::beforeUpdate(const NeedCalculationDetail& d) {
//     // Ugyanaz a szabály, mint insertnél
//     if (!isFormulaValid(d.formula)) {
//         zWarning(QString("⚠️ NeedCalculationDetail update: invalid formula: %1")
//                      .arg(d.formula));
//         return false;
//     }

//     if (!materialExists(d.materialId)) {
//         zWarning(QString("⚠️ NeedCalculationDetail update: material not found: %1")
//                      .arg(d.materialId.toString()));
//         return false;
//     }

//     return true;
// }

// void NeedCalculationDetailRegistry::onInsertLog(const NeedCalculationDetail& d) {
//     zInfo(QString("➕ NeedCalculationDetail INSERT: id=%1 material=%2 formula=%3")
//               .arg(d.id.toString(), d.materialId.toString(), d.formula));
// }

// void NeedCalculationDetailRegistry::onUpdateLog(const NeedCalculationDetail& d) {
//     zInfo(QString("✏️ NeedCalculationDetail UPDATE: id=%1 material=%2 formula=%3")
//               .arg(d.id.toString(), d.materialId.toString(), d.formula));
// }

// void NeedCalculationDetailRegistry::onRemoveLog(const NeedCalculationDetail& d) {
//     zInfo(QString("🗑️ NeedCalculationDetail REMOVE: id=%1 material=%2 formula=%3")
//               .arg(d.id.toString(), d.materialId.toString(), d.formula));
// }
