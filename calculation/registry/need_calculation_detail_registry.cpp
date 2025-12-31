#include "calculation/registry/need_calculation_detail_registry.h"
#include "calculation/repository/need_calculation_detail_repository.h"
#include "materials/registry/material_registry.h"

bool NeedCalculationDetailRegistry::isFormulaValid(const QString& f) {
    // Hunglish egyszerű validátor: engedélyezett minták
    // - w-<int>, h-<int>, fixed:<int>
    // Később: cparse vagy komolyabb parser.
    if (f.trimmed().isEmpty()) return false;
    if (f.startsWith("w-") || f.startsWith("h-")) {
        bool ok = false;
        f.mid(2).toInt(&ok);
        return ok;
    }
    if (f.startsWith("fixed:")) {
        bool ok = false;
        f.mid(QString("fixed:").size()).toInt(&ok);
        return ok;
    }
    return false;
}

bool NeedCalculationDetailRegistry::insert(const NeedCalculationDetail& det) {
    if (!isFormulaValid(det.formula)) {
        zWarning(QString("⚠️ NeedCalculationDetail insert: invalid formula: %1").arg(det.formula));
        return false;
    }
    // Material létezés check (auditbarát)
    auto* matRepoBase = RegistryManager::instance().findByTypeName("MaterialMaster");
    if (!matRepoBase) {
        zWarning("⚠️ NeedCalculationDetail insert: Material registry hiányzik");
        return false;
    }
    auto* matRepo = RegistryManager::instance().findByTypeName("MaterialMaster");
    if (!matRepo) {
        zWarning("⚠️ NeedCalculationDetail insert: Material registry hiányzik");
        return false;
    }

    if (!matRepo->findEntityById(det.materialId)) {
        zWarning(QString("⚠️ NeedCalculationDetail insert: material not found: %1")
                     .arg(det.materialId.toString()));
        return false;
    }


    _data.append(det);
    persist();
    zInfo(QString("➕ NeedCalculationDetail: material=%1 formula=%2")
                   .arg(det.materialId.toString(), det.formula));
    return true;
}

bool NeedCalculationDetailRegistry::remove(const QUuid& id) {
    auto it = std::remove_if(_data.begin(), _data.end(), [&](const NeedCalculationDetail& d){ return d.id == id; });
    if (it != _data.end()) {
        _data.erase(it, _data.end());
        persist();
        zInfo(QString("🗑️ NeedCalculationDetail removed: %1").arg(id.toString()));
        return true;
    }
    return false;
}

bool NeedCalculationDetailRegistry::updateFormula(const QUuid& id, const QString& newFormula) {
    if (!isFormulaValid(newFormula)) {
        zWarning(QString("⚠️ NeedCalculationDetail update: invalid formula: %1").arg(newFormula));
        return false;
    }
    for (auto& d : _data) {
        if (d.id == id) {
            d.formula = newFormula;
            persist();
            zInfo(QString("✏️ NeedCalculationDetail formula updated: %1").arg(id.toString()));
            return true;
        }
    }
    return false;
}

QVector<NeedCalculationDetail> NeedCalculationDetailRegistry::findByCalculation(const QUuid& calcId) const {
    QVector<NeedCalculationDetail> out;
    for (const auto& d : _data) if (d.needCalculationId == calcId) out.append(d);
    return out;
}

std::optional<NeedCalculationDetail> NeedCalculationDetailRegistry::findById(const QUuid& id) const {
    for (const auto& d : _data) if (d.id == id) return d;
    return std::nullopt;
}

void NeedCalculationDetailRegistry::persist() const {
    NeedCalculationDetailRepository::save(_data);
}
