#include "need_calculator.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "materials/registry/material_registry.h"
#include "kit_aggregator.h"

#include <expression/eval_result.h>
#include <expression/formula_engine.h>
#include <expression/variable.h>

#include "dsl/formula_analysis.h"
#include "dsl/formula_contract.h"


ItemNeed NeedCalculator::calculate(const OrderItem& item,
                                   const QString& modeName,
                                   bool debug)
{
    const NeedCalculation* calc =
        NeedCalculationRegistry::instance().findByProductAndName(item.productId, modeName);

    if (!calc) {
        zInfo(QString("No NeedCalculation for product %1, mode=%2")
                  .arg(item.productId.toString(), modeName));
        return {};
    }

    const auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

    if (details.isEmpty()) {
        zInfo(QString("No Details for product %1, mode=%2")
                  .arg(item.productId.toString(), modeName));
        return {};
    }

    ItemNeed i;

    for (const auto& d : details) {

        if (d.kind == NeedCalculationDetail::DetailKind::Cutting) {

            Result<RawCut> r = evalFormulaCut(item, d, debug);
            if (!r.ok) {
                zInfo(QString("⚠️ Cutting formula error in detail %1: %2")
                          .arg(d.id.toString(), r.error));
                continue;
            }

            i.cutItems.append(r.value);

        } else if (d.kind == NeedCalculationDetail::DetailKind::Kitting) {

            Result<RawKit> r = evalFormulaKit(item, d, debug);
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



// Cutting DSL értelmezés
Result<RawCut> NeedCalculator::evalFormulaCut(const OrderItem& item,
                                              const NeedCalculationDetail& detail,
                                              bool debug)
{
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

    fillVariables(item);

    EvalResult r = FormulaEngine::eval(detail.formula);
    if (debug) {
        r.debugDump();
    }

    if (!r.ok) {
        return Result<RawCut>::failure(
            QString("FormulaEngine error: %1").arg(r.error)
            );
    }

    RawCut raw;
    auto& vars = VariableRepository::instance();

    // requiredLength
    {
        Value v = vars.get("len");
        if (v.type() == Value::Type::Null)
            return Result<RawCut>::failure("len not set");
        raw.requiredLength = (int)v.number();
    }

    // qty (opcionális)
    {
        Value v = vars.get("qty");
        if (v.type() == Value::Type::Null)
            raw.cutting_qty = 1;
        else
            raw.cutting_qty = (int)v.number();
    }

    // material
    {
        Value v = vars.get("mat");
        if (v.type() == Value::Type::Null) {
            auto mat = MaterialRegistry::instance().findById(detail.materialId);
            if (!mat)
                return Result<RawCut>::failure("Fallback materialId invalid");
            raw.materialBarcode = mat->barcode;
        } else {
            if (v.type() != Value::Type::String)
                return Result<RawCut>::failure("material output must be string");
            raw.materialBarcode = v.string();
        }
    }

    return Result<RawCut>::success(raw);
}



// Kitting DSL értelmezés
Result<RawKit> NeedCalculator::evalFormulaKit(const OrderItem& item,
                                              const NeedCalculationDetail& detail,
                                              bool debug)
{
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

    fillVariables(item);

    EvalResult r = FormulaEngine::eval(detail.formula);
    if (debug) {
        r.debugDump();
    }

    if (!r.ok) {
        return Result<RawKit>::failure(
            QString("FormulaEngine error: %1").arg(r.error)
            );
    }

    RawKit raw;
    auto& vars = VariableRepository::instance();

    // qty
    {
        Value v = vars.get("qty");
        if (v.type() == Value::Type::Null)
            return Result<RawKit>::failure("qty not set");
        raw.kitting_qty = (int)v.number();
    }

    // material
    {
        Value v = vars.get("mat");
        if (v.type() == Value::Type::Null) {
            auto mat = MaterialRegistry::instance().findById(detail.materialId);
            if (!mat)
                return Result<RawKit>::failure("Fallback materialId invalid");
            raw.materialBarcode = mat->barcode;
        } else {
            if (v.type() != Value::Type::String)
                return Result<RawKit>::failure("material output must be string");
            raw.materialBarcode = v.string();
        }
    }

    return Result<RawKit>::success(raw);
}



// DSL változók feltöltése
void NeedCalculator::fillVariables(const OrderItem& item)
{
    auto& vars = VariableRepository::instance();
    vars.clear();

    vars.set("w",         Value::numberValue(item.width_mm));
    vars.set("h",         Value::numberValue(item.height_mm));
    vars.set("handler",   Value::stringValue(item.handlerSide));
    vars.set("owner",     Value::stringValue(item.ownerName));
    vars.set("color",     Value::stringValue(item.colorName));
    vars.set("externalId",Value::stringValue(item.externalId));
    vars.set("product",   Value::stringValue(item.productId.toString()));
}
