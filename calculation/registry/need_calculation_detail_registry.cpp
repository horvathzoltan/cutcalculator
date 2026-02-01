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
    // v2: empty = valid (unknown), "unknown" = invalid
    if (trimmed.isEmpty())
        return true;
    if (trimmed == "unknown")
        return false;

    // v2: len:* prefix → numeric suffix required
    if (trimmed.startsWith("len:w-") || trimmed.startsWith("len:h-")) {
        bool ok = false;
        trimmed.mid(6).toInt(&ok);
        return ok;
    }

    // v2: qty:* prefix → numeric suffix required
    // v2: qty:* prefix → numeric suffix required (fixed, perOrder, perArea)
    if (trimmed.startsWith("qty:fixed:") ||
        trimmed.startsWith("qty:perOrder:") ||
        trimmed.startsWith("qty:perArea:")) {
        bool ok = false;
        const int idx = trimmed.indexOf(':', 4) + 1;
        trimmed.mid(idx).toInt(&ok);
        return ok;
    }

    // v2: unknown prefix → invalid
    return false;
}


bool NeedCalculationDetailRegistry::materialExists(const QUuid& materialId) {
    const auto* ent =
        RegistryManager::instance().findEntity("MaterialMaster", materialId);
    return ent != nullptr;
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

// --- Domain hookok ---

// v2: trim formula before validation (empty = valid, "unknown" = invalid)
bool NeedCalculationDetailRegistry::beforeValidate(NeedCalculationDetail& d)
{
    d.formula = d.formula.trimmed();
    return true;
}

// v2: domain validation uses updated formula rules (isFormulaValid)
bool NeedCalculationDetailRegistry::validateDomain(const NeedCalculationDetail& d) const
{
    if(!isFormulaValid(d.formula)) return false;
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

bool NeedCalculationDetailRegistry::beforeInsert(NeedCalculationDetail& d)
{
    return materialExists(d.materialId);
}

bool NeedCalculationDetailRegistry::beforeUpdate(NeedCalculationDetail& d)
{
    return beforeInsert(d);
}

// --- Log hookok ---

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


