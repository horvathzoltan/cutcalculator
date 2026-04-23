#pragma once

#include <QString>
#include <QDate>
#include <QUuid>

#include "common/model/identifiable_entity.h"


/**
 * OrderHeader – a rendelés fejadatai
 *
 * Entitásként viselkedik:
 *  - saját technikai ID (IdentifiableEntity.id)
 *  - saját névmező (IdentifiableEntity.name) → customerName-be tükrözhető
 */

struct OrderHeader : public IdentifiableEntity {
    // Egyedi technikai azonosító a rendelés fejéhez.
    // Registry és repository számára szükséges.

    // IdentifiableEntity:
    // QUuid id;
    // QString name;  // ezt nem használjuk, de kötelező mező

    QString customerName;
    // A megrendelő neve.
    // Minden OrderItem.ownerName ezt örökli, hacsak nincs felülírva.

    QDate orderDate;
    // A rendelés dátuma (felvitel vagy megadott dátum).

    QString defaultColor;
    // A rendelés alapértelmezett színe.
    // Ha egy OrderItem nem ad meg saját colorName-et, ezt használjuk.

    QString defaultHandlerSide;
    // Alapértelmezett kezelőoldal ("L" vagy "R").
    // Ha egy OrderItem nem ad meg saját handlerSide-ot, ezt használjuk.

    QString defaultExternalPrefix;
    // Az externalId generálás alapja.
    // Ha egy OrderItem nem ad meg explicit externalId-t,
    // akkor OrderService generálja: prefix + sorszám.

    QString note;
    // Szabad szöveges megjegyzés a rendeléshez.

    QString status;
    // A rendelés státusza (pl. "draft", "confirmed", "in_production", stb.).

    QDate deadline;
    // A rendelés határideje.
    // A WorkOrder és a gyártási ütemezés számára fontos.
};
