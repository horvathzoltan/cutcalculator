#pragma once

#include <QString>
#include <utility>

// Ha itt akarod használni a logger API-t, ezt nyisd ki:
// #include "common/logger/logger.h"

// 🧩 BaseWorkflowPolicy
// - minden más workflow policy (CRUD, Connection, Barcode, Lookup) közös alapja
// - NEM tud CRUD-ról, kapcsolatokról, barcódról – csak egy általános "művelet keretet" ad

struct BaseWorkflowPolicy
{
    // 🔹 Általános keret egy névvel ellátott művelethez
    //
    // Használat:
    //   return BaseWorkflowPolicy::run("insert", [&]{
    //       // ... művelet lépései ...
    //       return true;
    //   });
    //
    template<typename Fn>
    static bool run(const QString& opName, Fn&& fn)
    {
        Q_UNUSED(opName);
        // Később ide kerülhet globális try/catch, tranzakció, context push, stb.
        return std::forward<Fn>(fn)();
    }

    // 🔹 Művelet előtti hook
    //
    // Itt lehetne:
    //  - verbose / audit context nyitás
    //  - "opName" logolása
    //
    template<typename Registry>
    static void beforeOperation(Registry& /*reg*/, const QString& /*opName*/)
    {
        // Alapértelmezésben nem csinál semmit.
        // Később: verbose/audit keret ide jöhet.
    }

    // 🔹 Művelet utáni hook
    //
    template<typename Registry>
    static void afterOperation(Registry& /*reg*/, const QString& /*opName*/)
    {
        // Alapértelmezésben nem csinál semmit.
        // Később: context zárás, összegző log, stb.
    }

    // 🔹 Persist hívás egységesítése
    //
    // Elvárás: Registry-nek legyen persist() metódusa
    // (akár üres implementáció is lehet, ha nincs perzisztálás).
    //
    template<typename Registry>
    static void persist(Registry& reg)
    {
        reg.persist();
    }

    // 🔹 (Opcionális) log helper függvények
    //
    // Ha nem akarod a loggert ide kötni, ezeket akár ki is hagyhatod,
    // vagy csak ott használod, ahol tényleg kell.

    static void logInfo(const QString& msg)
    {
        // Ha akarod, nyisd ki fent a logger include-ot, és:
        // zInfo(msg);
        Q_UNUSED(msg);
    }

    static void logWarn(const QString& msg)
    {
        // zWarning(msg);
        Q_UNUSED(msg);
    }

    static void logError(const QString& msg)
    {
        // zError(msg);
        Q_UNUSED(msg);
    }
};
