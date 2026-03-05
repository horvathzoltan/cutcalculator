#pragma once

#include <QUuid>
#include <QString>
#include <QVector>

#include "calculation/model/need_calculation_detail.h"
#include "calculation/service/piece.h"
#include "calculation/service/cut_aggregator.h"
#include "calculation/service/kit_aggregator.h"
#include "raw_cut.h"
#include "raw_kit.h"

#include "common/utils/result.h"

// v2 OrderLine – p3-ban töltjük ki
struct OrderLine {
    QUuid productId;

    int width_mm = 0;
    int height_mm = 0;
    int qty = 1;

    QString handlerSide;   // "L" / "R" vagy üres
    QString externalId;    // pl. "2650"
    QString ownerName;     // megrendelő neve
    QString colorName;     // szín megnevezése
};

class NeedCalculator {
public:
    // Cutting pipeline (v2)
    static QVector<CutAggregatedItem> makeCutList(const OrderLine& line,
                                                  const QString& modeName);

    // Kitting pipeline (v2)
    static QVector<KitAggregatedItem> makeKitList(const OrderLine& line,
                                        const QString& modeName);

private:
private:
    // Cutting DSL értelmezés
    static Result<RawCut> evalFormulaCut(const OrderLine& line,
                                 const NeedCalculationDetail& detail);

    // Kitting DSL értelmezés
    static Result<RawKit> evalFormulaKit(const OrderLine& line,
                                 const NeedCalculationDetail& detail);

    // Közös: DSL változók feltöltése
    static void fillVariables(const OrderLine& line);


    // RawCut → Piece lista (p9)
    static Result<QVector<Piece>> explodePieces(const OrderLine& line,
                                        const RawCut& raw);

};
