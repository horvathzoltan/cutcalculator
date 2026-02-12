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

    // v2: empty = valid (unknown), "unknown" = invalid
    // v2: minden rule-hoz legyen detail + formula "" → valid, "unknown" → invalid
    for (const auto& r : rules) {
        bool ok = false;
        for (const auto& d : details) {
            if (d.materialId == r.rightId &&
                NeedCalculationDetailRegistry::isFormulaValid(d.formula)) {
                ok = true;
                break;
            }
        }
        if (!ok) return false;
    }

    return true;
}

bool MatrixValidator::isProductMatrixComplete(const QUuid& productId)
{
    const auto modes = NeedCalculationRegistry::instance().findAll(
        [&](const NeedCalculation& nc){ return nc.productId == productId; });

    if (modes.isEmpty()) return false;

    // v2: product matrix complete if no mode has missing details
    return validateProduct(productId).isEmpty();
}


QVector<MissingDetail> MatrixValidator::validateProduct(const QUuid& productId)
{
    QVector<MissingDetail> out;

    const auto modes =
        NeedCalculationRegistry::instance().findAll(
            [&](const NeedCalculation& nc){ return nc.productId == productId; });

    for (const auto& m : modes) {
        auto v = validateMode(m.id);
        out += v;
    }

    return out;
}

QVector<MissingDetail> MatrixValidator::validateAll()
{
    QVector<MissingDetail> out;

    const auto modes = NeedCalculationRegistry::instance().readAll();
    for (const auto& m : modes) {
        auto v = validateMode(m.id);
        out += v;
    }

    return out;
}

// Emberi diagnosztikai log – MissingDetail mezők alapján
void MatrixValidator::logHumanReadableDiagnostics()
{
    auto diag = validateAll();
    for (const auto& m : diag) {
        zInfo(QString("❌ Missing detail → Product: %1 (%2), Mode: %3, Material: %4 (%5)")
                  .arg(m.productName)
                  .arg(m.productId.toString())
                  .arg(m.modeName)
                  .arg(m.materialName)
                  .arg(m.materialBarcode));
    }
}

QVector<MissingDetail> MatrixValidator::validateMode(const QUuid& modeId)
{
    QVector<MissingDetail> out;

    const NeedCalculation *mode =
        NeedCalculationRegistry::instance().findById(modeId);
    if (!mode) return out;

    const auto rules =
        NeedRuleRegistry::instance().findByLeft(mode->productId);

    const auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(modeId);

    const auto* product =
        ProductRegistry::instance().findById(mode->productId);

        QSet<QUuid> existing;
    for (const auto& d : details)
        existing.insert(d.materialId);

    for (const auto& r : rules) {
        if (!existing.contains(r.rightId)) {
            const auto* mat =
                MaterialRegistry::instance().findById(r.rightId);

            out.push_back({
                mode->productId,
                product ? product->name : QString("unknown"),
                modeId,
                mode->name,
                r.rightId,
                mat ? mat->name : QString("unknown"),
                mat ? mat->barcode : QString("unknown")
            });
        }
    }

    return out;
}


