#pragma once

#include <QString>
#include <QDate>

/**
 * OrderHeader – a rendelés fejadatai.
 *
 * Ezek azok az információk, amelyeket a user egyszer ad meg,
 * és minden OrderItem örökli őket (ownerName, defaultColor, stb.).
 *
 * A header NEM tartalmaz tételadatokat – csak a rendelés szintű metaadatokat.
 */
struct OrderHeader {

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
