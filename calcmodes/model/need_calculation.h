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
    QUuid productDefinitionId;  // 🔗 Kapcsolat a ProductMaster-hez

    QString toString() const override {
        return QString("NeedCalculation{id=%1, name=%2, productId=%3}")
            .arg(id.toString(), name, productDefinitionId.toString());
    }
};
