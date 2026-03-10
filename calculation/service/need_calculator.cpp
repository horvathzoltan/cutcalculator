#include "need_calculator.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "materials/registry/material_registry.h"
//#include "common/logger/event_logger.h"
//#include "cut_key.h"
#include "kit_aggregator.h"

#include <expression/eval_result.h>
#include <expression/formula_engine.h>
#include <expression/variable.h>

#include "dsl/formula_analysis.h"
#include "dsl/formula_contract.h"


ItemNeed NeedCalculator::calculate(const OrderLine& line, const QString& modeName, bool debug)
{
    //   QVector<CutAggregatedItem> out;

    const NeedCalculation* calc =
        NeedCalculationRegistry::instance().findByProductAndName(line.productId, modeName);

    if (!calc) {
        zInfo(QString("No NeedCalculation for product %1, mode=%2")
                  .arg(line.productId.toString(), modeName));
        // return out;
        return {};
    }

    const auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

    if (details.isEmpty()) {
        zInfo(QString("No Details for product %1, mode=%2")
                  .arg(line.productId.toString(), modeName));
        // return out;
        return {};
    }

    ItemNeed i;

    for (const auto& d : details) {
        if (d.kind == NeedCalculationDetail::DetailKind::Cutting){

            Result<RawCut> r = evalFormulaCut(line, d, debug);
            if (!r.ok) {
                zInfo(QString("⚠️ Cutting formula error in detail %1: %2")
                          .arg(d.id.toString(), r.error));
                continue;
            }

            i.cutItems.append(r.value);
        } else if (d.kind == NeedCalculationDetail::DetailKind::Kitting){
            Result<RawKit> r = evalFormulaKit(line, d, debug);

            if (!r.ok) {
                zInfo(QString("⚠️ Kitting formula error in detail %1: %2")
                          .arg(d.id.toString(), r.error));
                continue;
            }

            i.kitItems.append(r.value);
        }
    }
    return i;
}


// // p7 – implementáljuk később
// QVector<CutAggregatedItem>
// NeedCalculator::makeCutList(const OrderLine& line, const QString& modeName, bool debug)
// {
//  //   QVector<CutAggregatedItem> out;

//     const NeedCalculation* calc =
//         NeedCalculationRegistry::instance().findByProductAndName(line.productId, modeName);

//     if (!calc) {
//         zInfo(QString("No NeedCalculation for product %1, mode=%2")
//                        .arg(line.productId.toString(), modeName));
//        // return out;
//         return {};
//     }

//     const auto details =
//         NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

//     QVector<Piece> allPieces;

//     for (const auto& d : details) {
//         if (d.kind != NeedCalculationDetail::DetailKind::Cutting)
//             continue;

//         Result<RawCut> r = evalFormulaCut(line, d, debug);
//         if (!r.ok) {
//             zInfo(QString("⚠️ Cutting formula error in detail %1: %2")
//                       .arg(d.id.toString(), r.error));
//             continue;
//         }

//         RawCut raw = r.value;

//         auto piecesRes = explodePieces(line, raw);
//         if (!piecesRes.ok) {
//             zInfo(QString("⚠️ explodePieces error in detail %1: %2")
//                       .arg(d.id.toString(), piecesRes.error));
//             continue;
//         }

//         allPieces += piecesRes.value;

//     }

//     // p11 – CutAggregator integráció
//     return CutAggregator::aggregate(allPieces);
// }



// p8 – implementáljuk később
// QVector<KitAggregatedItem>
// NeedCalculator::makeKitList(const OrderLine& line, const QString& modeName, bool debug)
// {
//     //QVector<KitItem> out;

//     // 1) NeedCalculation betöltése
//     const NeedCalculation* calc =
//         NeedCalculationRegistry::instance().findByProductAndName(line.productId, modeName);

//     if (!calc) {
//         zInfo(QString("No NeedCalculation for product %1, mode=%2")
//                        .arg(line.productId.toString(), modeName));
//         return {};
//     }

//     // 2) Detail sorok betöltése
//     const auto details =
//         NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

//     QVector<KitItem> out;
//     // 3) Minden detail sor feldolgozása
//     for (const auto& d : details) {

//         // Csak Kitting sorok
//         if (d.kind != NeedCalculationDetail::DetailKind::Kitting)
//             continue;

//         // 3.1) DSL értelmezése → RawKit
//         auto r = evalFormulaKit(line, d, debug);

//         if (!r.ok) {
//             zInfo(QString("⚠️ Kitting formula error in detail %1: %2")
//                       .arg(d.id.toString(), r.error));
//             continue;
//         }

//         RawKit raw = r.value;

//         // 3.2) KitItem összeállítása
//         KitItem item;
//         item.materialId = d.materialId;
//         item.materialBarcode = raw.materialBarcode;
//         item.quantity   = raw.qty;

//         item.ownerName  = line.ownerName;
//         item.colorName  = line.colorName;
//         item.fullWidth  = line.width_mm;
//         item.fullHeight = line.height_mm;

//         out.append(item);

//     }

//     return KitAggregator::aggregate(out);
// }



// Cutting DSL értelmezés
Result<RawCut> NeedCalculator::evalFormulaCut(const OrderLine& line,
                                              const NeedCalculationDetail& detail,
                                              bool debug)
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

    if(debug){
        r.debugDump();
    }

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
    // {
    //     Value v = vars.get("qty");
    //     if (v.type == Value::Type::Null)
    //         return Result<RawCut>::failure("qty not set");
    //     raw.qty = (int)v.toDouble();
    // }

    // 6) material
    // 6) material
    {
        Value v = vars.get("material");
        if (v.type == Value::Type::Null) {
            // fallback
            auto mat = MaterialRegistry::instance().findById(detail.materialId);
            if (!mat)
                return Result<RawCut>::failure("Fallback materialId invalid");
            raw.materialBarcode = mat->barcode;
        } else {
            if (v.type != Value::Type::String)
                return Result<RawCut>::failure("material output must be string");
            raw.materialBarcode = v.text;
        }
    }


    return Result<RawCut>::success(raw);
}



// Kitting DSL értelmezés
Result<RawKit> NeedCalculator::evalFormulaKit(const OrderLine& line,
                                              const NeedCalculationDetail& detail,
                                              bool debug)
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
    if(debug){
        r.debugDump();
    }

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
            // fallback
            auto mat = MaterialRegistry::instance().findById(detail.materialId);
            if (!mat)
                return Result<RawKit>::failure("Fallback materialId invalid");
            raw.materialBarcode = mat->barcode;
        } else {
            if (v.type != Value::Type::String)
                return Result<RawKit>::failure("material output must be string");
            raw.materialBarcode = v.text;
        }
    }

     return Result<RawKit>::success(raw);
}

// p9 – implementáljuk később
// Result<QVector<Piece>>
// NeedCalculator::explodePieces(const OrderLine& line, const RawCut& raw)
// {
//     QVector<Piece> out;

//     // Cutting esetén a darabszám = rendelési sor darabszáma
//     int count = line.qty;
//     out.reserve(count);

//     QString barcode = raw.materialBarcode;

//     for (int i = 1; i <= count; ++i) {
//         Piece p;
//         //p.materialId      = raw.materialId;
//         //p.materialBarcode = raw.materialBarcode;
//         p.materialBarcode = barcode;
//         p.requiredLength  = raw.requiredLength;
//         p.handlerSide     = line.handlerSide;
//         p.externalRef     = QString("%1.%2").arg(line.externalId).arg(i);
//         p.ownerName       = line.ownerName;
//         p.colorName       = line.colorName;
//         p.fullWidth       = line.width_mm;
//         p.fullHeight      = line.height_mm;

//         out.append(p);
//     }

//     return Result<QVector<Piece>>::success(out);
// }


void NeedCalculator::fillVariables(const OrderLine& line)
{
    auto& vars = VariableRepository::instance();
    vars.clear();

    vars.set("w",        Value::numberValue(line.width_mm));
    vars.set("h",        Value::numberValue(line.height_mm));
    //vars.set("qty",      Value::numberValue(line.qty));
    vars.set("handler",  Value::stringValue(line.handlerSide));
    vars.set("owner",    Value::stringValue(line.ownerName));
    vars.set("color",    Value::stringValue(line.colorName));
    vars.set("externalId", Value::stringValue(line.externalId));
    vars.set("product",  Value::stringValue(line.productId.toString()));
}

