#include "need_calculator.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "materials/registry/material_registry.h"
#include "common/logger/event_logger.h"
#include "cut_key.h"

// p7 – implementáljuk később
QVector<CutAggregatedItem>
NeedCalculator::makeCutList(const OrderLine& line, const QString& modeName)
{
    QVector<CutAggregatedItem> out;

    const NeedCalculation* calc =
        NeedCalculationRegistry::instance().findByProductAndName(line.productId, modeName);

    if (!calc) {
        zEventWARN(QString("No NeedCalculation for product %1, mode=%2")
                       .arg(line.productId.toString(), modeName));
        return out;
    }

    auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

    QVector<Piece> allPieces;

    for (const auto& d : details) {
        if (d.kind != NeedCalculationDetail::DetailKind::Cutting)
            continue;

        try {
            RawCut raw = evalDsl(line, d);
            QVector<Piece> pieces = explodePieces(line, raw);
            allPieces += pieces;

        } catch (const QString& err) {
            zEventWARN(QString("Cutting formula error in detail %1: %2")
                           .arg(d.id.toString(), err));
            continue;
        }
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
        zEventWARN(QString("No NeedCalculation for product %1, mode=%2")
                       .arg(line.productId.toString(), modeName));
        return out;
    }

    // 2) Detail sorok betöltése
    auto details =
        NeedCalculationDetailRegistry::instance().findByCalculation(calc->id);

    // 3) Minden detail sor feldolgozása
    for (const auto& d : details) {

        // Csak Kitting sorok
        if (d.kind != NeedCalculationDetail::DetailKind::Kitting)
            continue;

        try {
            // 3.1) DSL értelmezése → RawCut
            RawCut raw = evalDsl(line, d);

            // 3.2) KitItem összeállítása
            KitItem item;
            item.materialId = raw.materialId;
            item.quantity   = raw.qty;

            item.ownerName  = line.ownerName;
            item.colorName  = line.colorName;
            item.fullWidth  = line.width_mm;
            item.fullHeight = line.height_mm;

            out.append(item);

        } catch (const QString& err) {
            zEventWARN(QString("Kitting formula error in detail %1: %2")
                           .arg(d.id.toString(), err));
            continue;
        }
    }

    return out;
}


// p6 – implementáljuk később
RawCut NeedCalculator::evalFormula(const OrderLine& line,
                                   const NeedCalculationDetail& detail)
{
    // 1) Változók törlése és feltöltése
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

    // 2) FormulaEngine futtatása
    EvalResult r = FormulaEngine::eval(detail.formula);
    if (!r.ok)
        throw QString("FormulaEngine error in detail %1: %2")
            .arg(detail.id.toString(), r.error);

    // 3) Eredmények kiolvasása
    RawCut raw;

    // requiredLength
    {
        Value v = vars.get("requiredLength");
        if (v.type == Value::Type::Null)
            throw QString("Formula did not set requiredLength in detail %1")
                .arg(detail.id.toString());
        raw.requiredLength = (int)v.toDouble();
    }

    // qty
    {
        Value v = vars.get("qty");
        if (v.type == Value::Type::Null)
            throw QString("Formula did not set qty in detail %1")
                .arg(detail.id.toString());
        raw.qty = (int)v.toDouble();
    }

    // material
    {
        Value v = vars.get("material");
        if (v.type == Value::Type::Null) {
            // fallback: detail.materialId
            raw.materialId = detail.materialId;
        } else {
            QString barcode = v.toString();
            auto mat = MaterialRegistry::instance().findByBarcode(barcode);
            if (!mat)
                throw QString("Invalid material barcode '%1' in detail %2")
                    .arg(barcode, detail.id.toString());
            raw.materialId = mat->id;
        }
    }

    return raw;
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


// p10 – implementáljuk később
// QVector<CutAggregatedItem>
// NeedCalculator::groupByCutKey(const QVector<Piece>& pieces)
// {
//     QMap<CutKey, CutAggregatedItem> map;

//     for (const Piece& p : pieces) {

//         // 1) Kulcs előállítása
//         CutKey key;
//         key.materialBarcode = p.materialBarcode;
//         key.requiredLength  = p.requiredLength;
//         key.handlerSide     = p.handlerSide;

//         // 2) Ha nincs ilyen kulcs → új aggregált elem
//         if (!map.contains(key)) {
//             CutAggregatedItem item;
//             item.materialBarcode = p.materialBarcode;
//             item.requiredLength  = p.requiredLength;
//             item.handlerSide     = p.handlerSide;
//             item.quantity        = 1;

//             item.externalRefs = QStringList{ p.externalRef };
//             item.ownerName    = p.ownerName;
//             item.fullWidth    = p.fullWidth;
//             item.fullHeight   = p.fullHeight;

//             map.insert(key, item);
//         }
//         else {
//             // 3) Ha már létezik → összevonás
//             auto& item = map[key];
//             item.quantity += 1;
//             item.externalRefs.append(p.externalRef);
//         }
//     }

//     // 4) QMap → QVector (determinista sorrend)
//     return map.values().toVector();
// }

