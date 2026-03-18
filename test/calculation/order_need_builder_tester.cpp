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

    // 2) Order összeállítása
    Order order;
    order.orderId = QUuid::createUuid();

    // --- 1. tétel: Rugós roletta, qty = 2 ---
    OrderItem it1;
    it1.orderItemId = QUuid::createUuid();
    it1.line.productId   = ids.ROL_R;
    it1.line.width_mm    = 1200;
    it1.line.height_mm   = 1500;
    it1.line.handlerSide = "L";
    it1.line.externalId  = "X1";
    it1.line.ownerName   = "Teszt Elek";
    it1.line.colorName   = "Fehér";
    it1.qty = 2;
    it1.modeName = "Manufacturing";

    order.items.append(it1);

    // --- 2. tétel: Tetőtéri roletta, qty = 1 ---
    OrderItem it2;
    it2.orderItemId = QUuid::createUuid();
    it2.line.productId   = ids.ROL_RT;
    it2.line.width_mm    = 1000;
    it2.line.height_mm   = 1400;
    it2.line.handlerSide = "R";
    it2.line.externalId  = "X2";
    it2.line.ownerName   = "Teszt Elek";
    it2.line.colorName   = "Fehér";
    it2.qty = 1;
    it2.modeName = "Manufacturing";

    order.items.append(it2);

    // 3) OrderNeed előállítása
    OrderNeed need = OrderNeedBuilder::build(order, true);

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
    // Legalább egy anyagból több darabnak kell összeadódnia
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
