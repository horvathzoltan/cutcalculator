#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/manager/registry_manager.h"
#include "calculation/repository/need_calculation_detail_repository.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"


// --- Segédek ---

bool NeedCalculationDetailRegistry::isFormulaValid(const QString& f) {
    const auto trimmed = f.trimmed();

    if (trimmed.isEmpty())
        return true;
    if (trimmed == "unknown")
        return false;

    // v2 DSL
    if (trimmed.startsWith("len:w-") || trimmed.startsWith("len:h-")) {
        bool ok = false;
        trimmed.mid(6).toInt(&ok);
        return ok;
    }

    if (trimmed.startsWith("qty:fixed:") ||
        trimmed.startsWith("qty:perOrder:") ||
        trimmed.startsWith("qty:perArea:")) {
        bool ok = false;
        const int idx = trimmed.indexOf(':', 4) + 1;
        trimmed.mid(idx).toInt(&ok);
        return ok;
    }

    // v1 DSL (FormulaEngine kompatibilitás)
    if (trimmed.startsWith("w-") || trimmed.startsWith("h-")) {
        bool ok = false;
        trimmed.mid(2).toInt(&ok);
        return ok;
    }

    if (trimmed.startsWith("fixed:")) {
        bool ok = false;
        trimmed.mid(6).toInt(&ok);
        return ok;
    }

    if (trimmed == "w*h") {
        return true;
    }

    return false;
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

    return updateWithWorkflow(updated);
}


// --- Lookup API ---

QVector<NeedCalculationDetail>
NeedCalculationDetailRegistry::findByCalculation(const QUuid& calcId) const
{
    return findAll([&](const NeedCalculationDetail& d){
        return d.needCalculationId == calcId;
    });
}

bool NeedCalculationDetailRegistry::insert(const NeedCalculationDetail &d) {
    return insertWithWorkflow(d);
}

bool NeedCalculationDetailRegistry::update(const NeedCalculationDetail &d) {
    return updateWithWorkflow(d);
}

bool NeedCalculationDetailRegistry::remove(const QUuid &id) {
    return removeWithWorkflow(id);
}

// --- Domain hookok ---

// v2: domain validation uses updated formula rules (isFormulaValid)
bool NeedCalculationDetailRegistry::validateDomain(const NeedCalculationDetail& d) const
{
    bool ifv = isFormulaValid(d.formula);
    if(!ifv) return false;
    if(d.kind != NeedCalculationDetail::DetailKind::Cutting &&
        d.kind != NeedCalculationDetail::DetailKind::Kitting) return false;

    if(d.materialId.isNull()) return false;
    if(d.needCalculationId.isNull()) return false;

    const NeedCalculation *nc =
        NeedCalculationRegistry::instance().findById(d.needCalculationId);

    if(!nc) return false;

    auto m = MaterialRegistry::instance().findById(d.materialId);
    if(!m) return false;

    auto p = ProductRegistry::instance().findById(nc->productId);
    if(!p) return false;

    return true;
}

bool NeedCalculationDetailRegistry::validateDuplicate(const NeedCalculationDetail& d) const
{
    return !existsBy([&](const NeedCalculationDetail& x){
        return x.needCalculationId == d.needCalculationId
               && x.materialId == d.materialId
               && x.id != d.id;
    });
}

bool NeedCalculationDetailRegistry::beforeInsert(NeedCalculationDetail &d) { return validateMaterial(d); }

bool NeedCalculationDetailRegistry::beforeUpdate(NeedCalculationDetail &d) { return validateMaterial(d); }

/*
// --- Shadow-mode előkészítés ---
// A log hookok teljesen mellékhatás-mentesek (csak zInfo).
// A persist() tiszta, nem módosít memóriát és nem hív CRUD-ot.
// A registry készen áll a log→persist sorrend későbbi átállítására (WF-1/WF-2).
*/

// v2: log reflects formula state (empty=valid, "unknown"=invalid)
void NeedCalculationDetailRegistry::onInsertLog(const NeedCalculationDetail& d)
{
    zInfo(QString("➕ NeedCalculationDetail INSERT: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

// v2: log reflects updated formula state
void NeedCalculationDetailRegistry::onUpdateLog(const NeedCalculationDetail& d)
{
    zInfo(QString("✏️ NeedCalculationDetail UPDATE: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

// v2: log reflects formula state at removal
void NeedCalculationDetailRegistry::onRemoveLog(const NeedCalculationDetail& d)
{
    zInfo(QString("🗑️ NeedCalculationDetail REMOVE: id=%1 material=%2 formula=%3")
              .arg(d.id.toString(), d.materialId.toString(), d.formula));
}

/*
// --- AfterHook-ok ---
// Mellékhatás-mentes, opcionális UI/notification lépések.
// Workflow sorrend: persist → log → afterHook.
void afterInsert(const NeedCalculationDetail&);
void afterUpdate(const NeedCalculationDetail&);
void afterRemove(const NeedCalculationDetail&);
*/
void NeedCalculationDetailRegistry::afterInsert(const NeedCalculationDetail& d)
{
    // Mellékhatás-mentes: opcionális UI/diagnosztikai jelzés
    zInfo(QString("ℹ️ NeedCalculationDetail AFTER INSERT: id=%1")
              .arg(d.id.toString()));
}

void NeedCalculationDetailRegistry::afterUpdate(const NeedCalculationDetail& d)
{
    zInfo(QString("ℹ️ NeedCalculationDetail AFTER UPDATE: id=%1")
              .arg(d.id.toString()));
}

void NeedCalculationDetailRegistry::afterRemove(const NeedCalculationDetail& d)
{
    zInfo(QString("ℹ️ NeedCalculationDetail AFTER REMOVE: id=%1")
              .arg(d.id.toString()));
}

// --- Persist ---

void NeedCalculationDetailRegistry::persist() const
{
    NeedCalculationDetailRepository::save(readAll());
}

// v2: registry load log reflects formula model v2 state
void NeedCalculationDetailRegistry::onLoadLog()
{
    zInfo(QString("📊 NeedCalculationDetailRegistry: %1 sor betöltve").arg(size()));
}

bool NeedCalculationDetailRegistry::validateMaterial(const NeedCalculationDetail& d)
{
    return materialExists(d.materialId);
}

bool NeedCalculationDetailRegistry::materialExists(const QUuid& materialId) {
    const auto* ent =
        RegistryManager::instance().findEntity("MaterialMaster", materialId);
    return ent != nullptr;
}

