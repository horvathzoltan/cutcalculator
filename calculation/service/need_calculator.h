#pragma once

#include <QUuid>
#include <QString>
#include <QVector>

#include "calculation/model/need_calculation_detail.h"
//#include "calculation/service/piece.h"
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
    //int qty = 1;

    QString handlerSide;   // "L" / "R" vagy üres
    QString externalId;    // pl. "2650"
    QString ownerName;     // megrendelő neve
    QString colorName;     // szín megnevezése
};

struct ItemNeed {
    QVector<RawCut> cutItems;  // nem aggregált
    QVector<RawKit> kitItems;  // nem aggregált
};

struct OrderItemNeed {
    QUuid orderItemId;
    QVector<ItemNeed> itemNeeds; // minden példány külön
};

struct OrderNeed {
    QUuid orderId;
    QVector<OrderItemNeed> orderItemNeeds;
};

class NeedCalculator {
public:

    static ItemNeed calculate(const OrderLine& line, const QString& modeName, bool debug);

    // Cutting pipeline (v2)
    // static QVector<CutAggregatedItem> makeCutList(const OrderLine& line,
    //                                               const QString& modeName,
    //                                               bool debug = false);

    // // Kitting pipeline (v2)
    // static QVector<KitAggregatedItem> makeKitList(const OrderLine& line,
    //                                               const QString& modeName,
    //                                               bool debug = false);

private:
    // Cutting DSL értelmezés
    static Result<RawCut> evalFormulaCut(const OrderLine& line,
                                         const NeedCalculationDetail& detail,
                                         bool debug = false);

    // Kitting DSL értelmezés
    static Result<RawKit> evalFormulaKit(const OrderLine& line,
                                         const NeedCalculationDetail& detai,
                                         bool debug = false);

    // Közös: DSL változók feltöltése
    static void fillVariables(const OrderLine& line);


    // RawCut → Piece lista (p9)
    // static Result<QVector<Piece>> explodePieces(const OrderLine& line,
    //                                     const RawCut& raw);

};
