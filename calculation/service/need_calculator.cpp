#include "expression/formula_engine.h"
#include "expression/variable.h"
#include "need_calculator.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "common/logger/event_logger.h"


QVector<CutItem> NeedCalculator::makeCutList(const OrderLine& ol, const QString& modeName) {
    QVector<CutItem> out;

    const NeedCalculation *calc =
        NeedCalculationRegistry::instance().findByProductAndName(ol.productId, modeName);
    if (!calc) {
        zEventWARN(QString("⚠️ No calculation mode for product %1").arg(ol.productId.toString()));
        return out;
    }

    const auto calcId = calc->id;
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    for (const auto& d : details) {

        if (d.kind != NeedCalculationDetail::DetailKind::Cutting)
            continue;

        auto& vars = VariableRepository::instance();
        vars.clear();
        vars.set("w", Value::numberValue(ol.w_mm));
        vars.set("h", Value::numberValue(ol.h_mm));
        vars.set("qty", Value::numberValue(ol.qty));

        auto r = FormulaEngine::eval(d.formula);
        if (!r.ok) {
            zEventWARN(QString("Formula error: %1").arg(r.error));
            continue;
        }

        Value result = vars.get("_result");

        // --- STRING eredmény → choose: anyag barcode ---
        if (result.type == Value::Type::String) {
            const auto* mat =
                MaterialRegistry::instance().findByBarcode(result.text);

            if (!mat)
                continue;

            out.append({ mat->id, 0, ol.qty });
            continue;
        }

        // --- NUMERIKUS eredmény ---
        if (result.type == Value::Type::Number) {
            int length = (int)result.number;
            int pieces = ol.qty; // régi logika: qty alapú darabszám

            if (length > 0 && pieces > 0)
                out.append({ d.materialId, length, pieces });
        }
    }

    return out;
}


QVector<KitItem> NeedCalculator::makeKitList(const OrderLine& ol, const QString& modeName) {
    QVector<KitItem> out;

    const NeedCalculation *calc =
        NeedCalculationRegistry::instance().findByProductAndName(ol.productId, modeName);
    if (!calc)
        return out;

    const auto calcId = calc->id;
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    for (const auto& d : details) {

        if (d.kind != NeedCalculationDetail::DetailKind::Kitting)
            continue;

        auto& vars = VariableRepository::instance();
        vars.clear();
        vars.set("w", Value::numberValue(ol.w_mm));
        vars.set("h", Value::numberValue(ol.h_mm));
        vars.set("qty", Value::numberValue(ol.qty));

        auto r = FormulaEngine::eval(d.formula);
        if (!r.ok) {
            zEventWARN(QString("Formula error: %1").arg(r.error));
            continue;
        }

        Value result = vars.get("_result");

        // --- STRING eredmény → choose: anyag barcode ---
        if (result.type == Value::Type::String) {
            const auto* mat =
                MaterialRegistry::instance().findByBarcode(result.text);

            if (!mat)
                continue;

            out.append({ mat->id, ol.qty });
            continue;
        }

        // --- NUMERIKUS eredmény → pieces ---
        if (result.type == Value::Type::Number) {
            int pieces = (int)result.number;
            if (pieces > 0)
                out.append({ d.materialId, pieces });
        }
    }

    return out;
}
