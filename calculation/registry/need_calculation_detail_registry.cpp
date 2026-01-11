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

bool NeedCalculationDetailRegistry::beforeInsert(NeedCalculationDetail& d)
{
    return materialExists(d.materialId);
}

bool NeedCalculationDetailRegistry::beforeUpdate(NeedCalculationDetail& d)
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

void NeedCalculationDetailRegistry::onLoadLog()
{
    zInfo(QString("📊 NeedCalculationDetailRegistry: %1 sor betöltve").arg(size()));
}


