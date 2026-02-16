#include "formula_engine.h"
#include "need_calculator.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "common/logger/event_logger.h"

QVector<CutItem> NeedCalculator::makeCutList(const OrderLine& ol, const QString& modeName) {
    QVector<CutItem> out;

    const NeedCalculation *calc =
        NeedCalculationRegistry::instance().findByProductAndName(ol.productId,modeName);
    if (!calc) {
        zEventWARN(QString("⚠️ No calculation mode for product %1").arg(ol.productId.toString()));
        return out;
    }

    const auto calcId = calc->id;
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    for (const auto& d : details) {
        // Heurisztika: formula w/h → cutting; fixed → kitting (másik listába)
        if (d.kind != NeedCalculationDetail::DetailKind::Cutting) continue;
        auto ev = FormulaEngine::eval(d.formula, ol.w_mm, ol.h_mm, ol.qty);

        // --- ÚJ: choose: operátor kezelése ---
        if (!ev.stringValue.isEmpty()) {
            // stringValue = anyag barcode
            const auto* mat =
                MaterialRegistry::instance().findByBarcode(ev.stringValue);

            if (!mat) {
                // hibás barcode → kihagyjuk
                continue;
            }

            // choose: mindig darabos anyag
            out.append({ mat->id, 0, ol.qty });
            continue;
        }

        // --- RÉGI numerikus logika ---
        if (ev.length_mm > 0 && ev.pieces > 0) {
            out.append({ d.materialId, ev.length_mm, ev.pieces });
        }

    }
    return out;
}

QVector<KitItem> NeedCalculator::makeKitList(const OrderLine& ol, const QString& modeName) {
    QVector<KitItem> out;
    const NeedCalculation *calc =
        NeedCalculationRegistry::instance().findByProductAndName(ol.productId, modeName);
    if (!calc) return out;
    const auto calcId = calc->id;
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    for (const auto& d : details) {
        if (d.kind == NeedCalculationDetail::DetailKind::Kitting) {
            auto ev = FormulaEngine::eval(d.formula, ol.w_mm, ol.h_mm, ol.qty);

            // --- ÚJ: choose: operátor ---
            if (!ev.stringValue.isEmpty()) {
                const auto* mat =
                    MaterialRegistry::instance().findByBarcode(ev.stringValue);
                if (!mat)
                    continue;

                out.append({ mat->id, ol.qty });
                continue;
            }

            // --- RÉGI logika ---
            if (ev.pieces > 0) {
                out.append({ d.materialId, ev.pieces });
            }

        }
    }
    return out;
}
