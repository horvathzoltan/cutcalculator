#include "order_need_builder_tester.h"

#include "calculation/service/order_need_builder.h"
#include "calculation/service/worksheet_builder.h"
#include "test/calculation/test_roletta_builder.h"

#include <QtGlobal>

/* ============================================================
 * 🧪 OrderNeedBuilderTester::run
 * ============================================================ */

#include "order_need_builder_tester.h"

#include "calculation/service/order_need_builder.h"
#include "calculation/service/worksheet_builder.h"
#include "test/calculation/test_roletta_builder.h"

#include <QtGlobal>

/* ============================================================
 * 🧪 OrderNeedBuilderTester::run
 * ============================================================ */

bool OrderNeedBuilderTester::run()
{
    zInfo("=== OrderNeedBuilder TEST START ===");

    // 1) Roletta tesztadatok előállítása
    auto ids = TestRolettaBuilder::build();

    // 2) OrderHeader + OrderItem lista összeállítása
    OrderHeader header;
    header.id = QUuid::createUuid();
    header.customerName = "Teszt Elek";
    //header.externalId = "ORD-TEST-001";

    QVector<OrderItem> items;

    // --- 1. tétel: Rugós roletta, qty = 2 ---
    {
        OrderItem it;
        it.id = QUuid::createUuid();
        it.orderId = header.id;

        it.productId   = ids.ROL_R;
        it.width_mm    = 1200;
        it.height_mm   = 1500;
        it.handlerSide = "L";
        it.externalId  = "X1";
        it.ownerName   = "Teszt Elek";
        it.colorName   = "Fehér";

        it.order_qty = 2;
        it.modeName = "Manufacturing";

        items.append(it);
    }

    // --- 2. tétel: Tetőtéri roletta, qty = 1 ---
    {
        OrderItem it;
        it.id = QUuid::createUuid();
        it.orderId = header.id;

        it.productId   = ids.ROL_RT;
        it.width_mm    = 1000;
        it.height_mm   = 1400;
        it.handlerSide = "R";
        it.externalId  = "X2";
        it.ownerName   = "Teszt Elek";
        it.colorName   = "Fehér";

        it.order_qty = 1;
        it.modeName = "Manufacturing";

        items.append(it);
    }

    // 3) OrderNeed előállítása
    OrderNeed need = OrderNeedBuilder::build(header.id, true);

    // --- Assert-ek: OrderNeed ---
    Q_ASSERT(need.orderItemNeeds.size() == 2);

    Q_ASSERT(need.orderItemNeeds[0].itemNeeds.size() == 2); // qty = 2
    Q_ASSERT(need.orderItemNeeds[1].itemNeeds.size() == 1); // qty = 1

    // 4) WorksheetNeed előállítása
    WorksheetNeed ws = WorksheetBuilder::fromOrderNeed(need);

    // --- Assert-ek: WorksheetNeed ---
    Q_ASSERT(!ws.cuts.isEmpty());
    Q_ASSERT(!ws.kits.isEmpty());

    // --- Aggregáció működik ---
    bool hasAggregatedCut = false;
    for (const auto& c : ws.cuts) {
        if (c.quantity >= 2) {
            hasAggregatedCut = true;
            break;
        }
    }
    Q_ASSERT(hasAggregatedCut);

    zInfo("=== OrderNeedBuilder TEST END ===");
    return true;
}
