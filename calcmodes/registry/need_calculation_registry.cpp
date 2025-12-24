#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/repository/need_calculation_repository.h"

bool NeedCalculationRegistry::exists(const QUuid& productId, const QString& modeName) const {
    for (const auto& c : _data) {
        if (c.productDefinitionId == productId && c.modeName.compare(modeName, Qt::CaseInsensitive) == 0)
            return true;
    }
    return false;
}

bool NeedCalculationRegistry::insert(const NeedCalculation& calc) {
    if (calc.modeName.trimmed().isEmpty()) {
        zWarning("⚠️ NeedCalculation insert: üres modeName nem engedélyezett");
        return false;
    }
    if (exists(calc.productDefinitionId, calc.modeName)) {
        zWarning(QString("⚠️ NeedCalculation insert: duplikált mód a terméken: %1").arg(calc.modeName));
        return false;
    }
    _data.append(calc);
    persist();
    zInfo(QString("➕ NeedCalculation: %1").arg(calc.modeName));
    return true;
}

bool NeedCalculationRegistry::remove(const QUuid& id) {
    auto it = std::remove_if(_data.begin(), _data.end(), [&](const NeedCalculation& c){ return c.id == id; });
    if (it != _data.end()) {
        _data.erase(it, _data.end());
        persist();
        zInfo(QString("🗑️ NeedCalculation removed: %1").arg(id.toString()));
        return true;
    }
    return false;
}

bool NeedCalculationRegistry::rename(const QUuid& id, const QString& newName) {
    if (newName.trimmed().isEmpty()) {
        zWarning("⚠️ NeedCalculation rename: üres új név tiltva");
        return false;
    }
    for (auto& c : _data) {
        if (c.id == id) {
            if (exists(c.productDefinitionId, newName)) {
                zWarning(QString("⚠️ NeedCalculation rename: duplikáció %1").arg(newName));
                return false;
            }
            c.modeName = newName;
            persist();
            zInfo(QString("✏️ NeedCalculation renamed: %1 → %2").arg(id.toString(), newName));
            return true;
        }
    }
    return false;
}

std::optional<NeedCalculation> NeedCalculationRegistry::findById(const QUuid& id) const {
    for (const auto& c : _data) if (c.id == id) return c;
    return std::nullopt;
}

QVector<NeedCalculation> NeedCalculationRegistry::findByProduct(const QUuid& productId) const {
    QVector<NeedCalculation> out;
    for (const auto& c : _data) if (c.productDefinitionId == productId) out.append(c);
    return out;
}

std::optional<NeedCalculation> NeedCalculationRegistry::findByProductAndName(const QUuid& productId, const QString& modeName) const {
    for (const auto& c : _data) {
        if (c.productDefinitionId == productId && c.modeName.compare(modeName, Qt::CaseInsensitive) == 0)
            return c;
    }
    return std::nullopt;
}

void NeedCalculationRegistry::persist() const {
    NeedCalculationRepository::save(_data);
}
