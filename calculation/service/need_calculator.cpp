#include "need_calculator.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "common/logger/event_logger.h"

int NeedCalculator::evalFormula(const QString& f, int w, int h) {
    if (f.startsWith("w-")) {
        bool ok = false; int sub = f.mid(2).toInt(&ok);
        return ok ? (w - sub) : w;
    }
    if (f.startsWith("h-")) {
        bool ok = false; int sub = f.mid(2).toInt(&ok);
        return ok ? (h - sub) : h;
    }
    if (f.startsWith("fixed:")) {
        bool ok = false; int val = f.mid(QString("fixed:").size()).toInt(&ok);
        return ok ? val : 0;
    }
    return 0;
}

QVector<CutItem> NeedCalculator::makeCutList(const OrderLine& ol, const QString& modeName) {
    QVector<CutItem> out;

    const NeedCalculation *calc =
        NeedCalculationRegistry::instance().findByProductAndName(ol.productId,
                                                                                           modeName);
    if (!calc) {
        zEventWARN(QString("⚠️ No calculation mode for product %1").arg(ol.productId.toString()));
        return out;
    }

    const auto calcId = calc->id;
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    for (const auto& d : details) {
        // Heurisztika: formula w/h → cutting; fixed → kitting (másik listába)
        if (d.formula.startsWith("fixed:")) continue; // kitting will consume
        int len = evalFormula(d.formula, ol.w_mm, ol.h_mm);
        if (len > 0) {
            out.append({ d.materialId, len, ol.qty });
        }
    }
    return out;
}

QVector<KitItem> NeedCalculator::makeKitList(const OrderLine& ol, const QString& modeName) {
    QVector<KitItem> out;
    const NeedCalculation *calc =
        NeedCalculationRegistry::instance().findByProductAndName(ol.productId,
                                                                                           modeName);
    if (!calc) return out;
    const auto calcId = calc->id;
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    for (const auto& d : details) {
        if (d.formula.startsWith("fixed:")) {
            int val = evalFormula(d.formula, ol.w_mm, ol.h_mm);
            if (val > 0) out.append({ d.materialId, val * ol.qty });
        }
    }
    return out;
}
