#include "matrix_validator.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "materials/registry/material_registry.h"

bool MatrixValidator::isCalculationMatrixComplete(const QUuid& calcId)
{
    const auto* calc = NeedCalculationRegistry::instance().findById(calcId);
    if (!calc) return false;

    const auto rules = NeedRuleRegistry::instance().findByLeft(calc->productId);
    const auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    // 1) minden rule-hoz legyen detail
    for (const auto& r : rules) {
        bool found = false;
        for (const auto& d : details) {
            if (d.materialId == r.rightId) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }

    // 2) minden detail legyen érvényes
    for (const auto& d : details) {
        if (d.materialId.isNull()) return false;
        if (d.formula.trimmed().isEmpty()) return false;

        if (!MaterialRegistry::instance().findById(d.materialId))
            return false;
    }

    return true;
}

bool MatrixValidator::isProductMatrixComplete(const QUuid& productId)
{
    const auto modes = NeedCalculationRegistry::instance().findAll(
        [&](const NeedCalculation& nc){ return nc.productId == productId; });

    if (modes.isEmpty()) return false;

    for (const auto& m : modes) {
        if (!isCalculationMatrixComplete(m.id))
            return false;
    }

    return true;
}
