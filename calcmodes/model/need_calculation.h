#pragma once
#include <QUuid>
#include <QString>

/**
 * 🧩 NeedCalculation – ProductDefinition ↔ CalculationMode kapcsolat
 *
 * Egy adott terméktípushoz tartozó számítási mód definíciója.
 * - Nem enum, hanem szabadon definiált string (pl. "falc", "glass").
 */
struct NeedCalculation {
    QUuid id;                   // 🆔 Technikai azonosító
    QUuid productDefinitionId;  // 🔗 Kapcsolat a ProductMaster-hez
    QString modeName;           // 📛 Számítási mód neve (pl. "falc")
};
