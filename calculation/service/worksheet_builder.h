#pragma once

#include <QVector>
#include <QUuid>

#include "order_need_builder.h"
#include "cut_aggregator.h"
#include "kit_aggregator.h"

/* ============================================================
 * 🧩 WorksheetNeed – aggregált vágási és kitting lista
 * ============================================================ */

struct WorksheetNeed {
    QUuid worksheetId;
    QVector<CutAggregatedItem> cuts;
    QVector<KitAggregatedItem> kits;
};

/* ============================================================
 * 🧩 WorksheetBuilder – OrderNeed → WorksheetNeed pipeline
 * ============================================================ */

class WorksheetBuilder {
public:
    static WorksheetNeed fromOrderNeed(const OrderNeed& orderNeed);
};
