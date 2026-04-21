#pragma once

#include <QUuid>
#include <QString>

/**
 * OrderItem – egyetlen megrendelt tétel teljes, véglegesített adatai.
 *
 * Ez a domain modell a valós rendelési sort írja le:
 * - a user ezt viszi fel,
 * - a NeedCalculator ezt eszi,
 * - a Worksheet és WorkOrder pipeline is ezt használja.
 *
 * NINCS többé OrderLine / CalcOrderLine kettősség.
 * Ez az egyetlen igazságforrás egy rendelési tételhez.
 */
struct OrderItem {
    QUuid id;
    // Egyedi technikai azonosító a rendelési tételhez.
    // A teljes pipeline (Need, Worksheet, WorkOrder) erre hivatkozik.

    QUuid productId;
    // A megrendelt termék típusa (ProductMaster.id).

    int width_mm = 0;
    int height_mm = 0;
    // A termék méretei milliméterben.
    // Ezeket a NeedCalculator közvetlenül használja a képletekben.

    QString handlerSide;
    // "L" vagy "R" – a kezelő oldala.
    // A user által megadott érték, nincs többé override-logika.

    QString externalId;
    // A tétel külső azonosítója (pl. 2650).
    // A user adhatja meg, vagy az OrderService generálja.

    QString ownerName;
    // A megrendelő neve – OrderHeader-ből öröklődik.
    // A NeedCalculator és a WorkOrder is használja.

    QString colorName;
    // A termék színe – véglegesített érték.
    // A user adja meg, vagy OrderHeader defaultból jön.

    int qty = 1;
    // Hány darabot rendeltek ebből a tételből.
    // A NeedCalculator minden példányra külön fut.

    QString modeName;
    // A számítási mód neve (pl. "standard", "premium", "tokos", stb.).
    // A NeedCalculator a productId + modeName alapján választja ki a képletet.
};
