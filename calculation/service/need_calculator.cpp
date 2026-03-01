#include "need_calculator.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "materials/registry/material_registry.h"
#include "common/logger/event_logger.h"
#include "cut_key.h"

#include <expression/eval_result.h>
#include <expression/formula_engine.h>
#include <expression/variable.h>

#include "dsl/formula_analysis.h"
#include "dsl/formula_contract.h"

// p7 – implementáljuk később
QVector<CutAggregatedItem>
NeedCalculator::makeCutList(const OrderLine& line, const QString& modeName)
{
    QVector<CutAggregatedItem> out;

    const NeedCalculation* calc =
        NeedCalculationRegistry::instance().findByProductAndName(line.productId, modeName);

    if (!calc) {
        zInfo(QString("No NeedCalculation for product %1, mode=%2")
                       .arg(line.productId.toString(), modeName));
        return out;
    }

    const auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

    QVector<Piece> allPieces;

    for (const auto& d : details) {
        if (d.kind != NeedCalculationDetail::DetailKind::Cutting)
            continue;

        auto r = evalFormulaCut(line, d);
        if (!r.ok) {
            zInfo(QString("⚠️ Cutting formula error in detail %1: %2")
                      .arg(d.id.toString(), r.error));
            continue;
        }

        RawCut raw = r.value;

        QVector<Piece> pieces = explodePieces(line, raw);
        allPieces += pieces;
    }

    // p11 – CutAggregator integráció
    return CutAggregator::aggregate(allPieces);
}



// p8 – implementáljuk később
QVector<KitItem>
NeedCalculator::makeKitList(const OrderLine& line, const QString& modeName)
{
    QVector<KitItem> out;

    // 1) NeedCalculation betöltése
    const NeedCalculation* calc =
        NeedCalculationRegistry::instance().findByProductAndName(line.productId, modeName);

    if (!calc) {
        zInfo(QString("No NeedCalculation for product %1, mode=%2")
                       .arg(line.productId.toString(), modeName));
        return out;
    }

    // 2) Detail sorok betöltése
    const auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

    // 3) Minden detail sor feldolgozása
    for (const auto& d : details) {

        // Csak Kitting sorok
        if (d.kind != NeedCalculationDetail::DetailKind::Kitting)
            continue;

        // 3.1) DSL értelmezése → RawKit
        auto r = evalFormulaKit(line, d);

        if (!r.ok) {
            zInfo(QString("⚠️ Kitting formula error in detail %1: %2")
                      .arg(d.id.toString(), r.error));
            continue;
        }

        RawKit raw = r.value;

        // 3.2) KitItem összeállítása
        KitItem item;
        item.materialId = raw.materialId;
        item.quantity   = raw.qty;

        item.ownerName  = line.ownerName;
        item.colorName  = line.colorName;
        item.fullWidth  = line.width_mm;
        item.fullHeight = line.height_mm;

        out.append(item);

    }

    return out;
}



// Cutting DSL értelmezés
Result<RawCut> NeedCalculator::evalFormulaCut(const OrderLine& line,
                                      const NeedCalculationDetail& detail)
{
    // 1) Contract validáció
    {
        FormulaContract contract = cuttingContract();
        FormulaAnalysis a = analyzeFormula(detail.formula, contract);
        if (!a.ok) {
            return Result<RawCut>::failure(
                QString("Contract validation failed: %1")
                    .arg(a.errors.join("; "))
                );
        }
    }

    // 2) Változók feltöltése
    fillVariables(line);

    // 3) Engine futtatása
    EvalResult r = FormulaEngine::eval(detail.formula);

    if (!r.ok) {
        return Result<RawCut>::failure(
            QString("FormulaEngine error: %1").arg(r.error)
            );
    }

    RawCut raw;
    auto& vars = VariableRepository::instance();

    // 4) requiredLength
    {
        Value v = vars.get("requiredLength");
        if (v.type == Value::Type::Null)
            return Result<RawCut>::failure("requiredLength not set");
        raw.requiredLength = (int)v.toDouble();
    }

    // 5) qty
    {
        Value v = vars.get("qty");
        if (v.type == Value::Type::Null)
            return Result<RawCut>::failure("qty not set");
        raw.qty = (int)v.toDouble();
    }

    // 6) material
    {
        Value v = vars.get("material");
        if (v.type == Value::Type::Null) {
            raw.materialId = detail.materialId;
        } else {
            QString barcode = v.toString();
            auto mat = MaterialRegistry::instance().findByBarcode(barcode);
            if (!mat)
                return Result<RawCut>::failure(
                    QString("Invalid material barcode '%1'").arg(barcode)
                    );
            raw.materialId = mat->id;
        }
    }

    return Result<RawCut>::success(raw);
}



// Kitting DSL értelmezés
Result<RawKit> NeedCalculator::evalFormulaKit(const OrderLine& line,
                                      const NeedCalculationDetail& detail)
{
    // 1) Contract validáció
    {
        FormulaContract contract = kittingContract();
        FormulaAnalysis a = analyzeFormula(detail.formula, contract);
        if (!a.ok) {
            return Result<RawKit>::failure(
                QString("Contract validation failed: %1")
                    .arg(a.errors.join("; "))
                );
        }
    }

    // 2) Változók feltöltése
    fillVariables(line);

    // 3) Engine futtatása
    EvalResult r = FormulaEngine::eval(detail.formula);

    if (!r.ok) {
        return Result<RawKit>::failure(
            QString("FormulaEngine error: %1").arg(r.error)
            );
    }
    RawKit raw;
    auto& vars = VariableRepository::instance();

    // 4) qty
    {
        Value v = vars.get("qty");
        if (v.type == Value::Type::Null)
            return Result<RawKit>::failure("qty not set");
        raw.qty = (int)v.toDouble();
    }

    // 5) material
    {
        Value v = vars.get("material");
        if (v.type == Value::Type::Null) {
            raw.materialId = detail.materialId;
        } else {
            QString barcode = v.toString();
            auto mat = MaterialRegistry::instance().findByBarcode(barcode);
            if (!mat)
                return Result<RawKit>::failure(
                    QString("Invalid material barcode '%1'").arg(barcode)
                    );
            raw.materialId = mat->id;
        }
    }

     return Result<RawKit>::success(raw);
}




// p9 – implementáljuk később
QVector<Piece>
NeedCalculator::explodePieces(const OrderLine& line, const RawCut& raw)
{
    QVector<Piece> out;
    out.reserve(raw.qty);

    // 1) Material lookup
    auto material = MaterialRegistry::instance().findById(raw.materialId);
    if (!material) {
        throw QString("Invalid materialId in RawCut.");
    }

    QString barcode = material->barcode;

    // 2) Darabok generálása
    for (int i = 1; i <= raw.qty; ++i) {
        Piece p;

        p.materialId      = raw.materialId;
        p.materialBarcode = barcode;
        p.requiredLength  = raw.requiredLength;
        p.handlerSide     = line.handlerSide;

        // 3) Egyedi externalRef
        // pl. "2650.1", "2650.2", ...
        p.externalRef = QString("%1.%2").arg(line.externalId).arg(i);

        // 4) Metaadatok
        p.ownerName  = line.ownerName;
        p.colorName  = line.colorName;
        p.fullWidth  = line.width_mm;
        p.fullHeight = line.height_mm;

        out.append(p);
    }

    return out;
}

void NeedCalculator::fillVariables(const OrderLine& line)
{
    auto& vars = VariableRepository::instance();
    vars.clear();

    vars.set("w",        Value::numberValue(line.width_mm));
    vars.set("h",        Value::numberValue(line.height_mm));
    vars.set("qty",      Value::numberValue(line.qty));
    vars.set("handler",  Value::stringValue(line.handlerSide));
    vars.set("owner",    Value::stringValue(line.ownerName));
    vars.set("color",    Value::stringValue(line.colorName));
    vars.set("externalId", Value::stringValue(line.externalId));
    vars.set("product",  Value::stringValue(line.productId.toString()));
}

