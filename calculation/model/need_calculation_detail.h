#pragma once
#include "common/model/identifiable_entity.h"

/**
 * 🧩 NeedCalculationDetail – CalculationMode ↔ Material + formula kapcsolat
 *
 * Egy adott számítási módon belül egy anyaghoz tartozó formula.
 * - Formula string: pl. "w-15", "fixed:2", "h-10".
 */
struct NeedCalculationDetail : public IdentifiableEntity {
    //QUuid id;                  // 🆔 Technikai azonosító
    QUuid needCalculationId;   // 🔗 Kapcsolat a NeedCalculation-hoz
    QUuid materialId;          // 🔗 Kapcsolat a MaterialMaster-hez
    QString formula;           // 📐 Számítási szabály string
};
