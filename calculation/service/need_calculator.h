#pragma once

#include <QUuid>
#include <QString>
#include <QVector>

#include "calculation/model/need_calculation_detail.h"
#include "calculation/service/piece.h"
#include "calculation/service/cut_aggregator.h"

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


// v2 KitItem – p4-ben töltjük ki
struct KitItem {
    QUuid materialId;
    int quantity;

    QString ownerName;
    QString colorName;

    int fullWidth;
    int fullHeight;
};


// v2 RawCut – p5-ben töltjük ki
struct RawCut {
    QUuid materialId;     // anyag azonosító
    int requiredLength;   // mm-ben
    int qty;              // darabszám
};


class NeedCalculator {
public:
    // Cutting pipeline (v2)
    static QVector<CutAggregatedItem> makeCutList(const OrderLine& line,
                                                  const QString& modeName);

    // Kitting pipeline (v2)
    static QVector<KitItem> makeKitList(const OrderLine& line,
                                        const QString& modeName);

private:
    // DSL értelmezés (p6)
    static RawCut evalFormula(const OrderLine& line,
                          const NeedCalculationDetail& detail);

    // RawCut → Piece lista (p9)
    static QVector<Piece> explodePieces(const OrderLine& line,
                                        const RawCut& raw);

    // Piece lista → aggregált vágási lista (p10)
    //static QVector<CutAggregatedItem> groupByCutKey(const QVector<Piece>& pieces);
};
