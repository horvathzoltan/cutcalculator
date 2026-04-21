#pragma once

#include <QUuid>
#include <QString>
#include <QVector>

#include "calculation/model/need_calculation_detail.h"
//#include "calculation/service/piece.h"
//#include "calculation/service/cut_aggregator.h"
//#include "calculation/service/kit_aggregator.h"
#include "orders/model/order_item.h"
#include "raw_cut.h"
#include "raw_kit.h"
#include "common/utils/result.h"

struct ItemNeed {
    QVector<RawCut> cutItems;  // nem aggregált
    QVector<RawKit> kitItems;  // nem aggregált
};

struct OrderItemNeed {
    QUuid orderItemId;
    OrderItem item;              // eredeti rendelési sor metaadatai
    QVector<ItemNeed> itemNeeds; // minden példány külön
};


struct OrderNeed {
    QUuid orderId;
    QVector<OrderItemNeed> orderItemNeeds;
};

class NeedCalculator {
public:

    static ItemNeed calculate(const OrderItem& line, const QString& modeName, bool debug);

private:
    // Cutting DSL értelmezés
    static Result<RawCut> evalFormulaCut(const OrderItem& line,
                                         const NeedCalculationDetail& detail,
                                         bool debug = false);

    // Kitting DSL értelmezés
    static Result<RawKit> evalFormulaKit(const OrderItem& line,
                                         const NeedCalculationDetail& detai,
                                         bool debug = false);

    // Közös: DSL változók feltöltése
    static void fillVariables(const OrderItem& line);
};