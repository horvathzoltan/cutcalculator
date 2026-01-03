#pragma once
#include <QUuid>
#include <QString>
#include "common/model/identifiable_entity.h"

/**
 * 🧩 NeedCalculation – ProductDefinition ↔ CalculationMode kapcsolat
 *
 * Egy adott terméktípushoz tartozó számítási mód definíciója.
 * - Nem enum, hanem szabadon definiált string (pl. "falc", "glass").
 */
struct NeedCalculation: public IdentifiableEntity {
    QUuid productId;  // 🔗 Kapcsolat a ProductMaster-hez
};
