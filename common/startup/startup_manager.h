#pragma once

#include "startup_status.h"

#include <QString>
#include <QStringList>
#include "common/logger/logger.h"

namespace StatusHelper{
inline QString getMessage(bool b, const QString& msg){
    return (b?"✅ ":"❌ ")+msg+(b?" ok":" sikertelen");
    }
}

// 🚀 Indítási logikát vezérlő osztály
// 🚀 Indítási logikát vezérlő osztály
class StartupManager {
public:
    StartupStatus runStartupSequence();

private:
    // ============================================================
    // 🧩 GENERIKUS REGISTRY BETÖLTŐ
    // ------------------------------------------------------------
    // ⚠️ Szerződés:
    // - CSV → memória betöltés (load-only path)
    // - NEM persistál vissza
    // - NEM futtat workflow-t vagy domain hookokat
    // - runtime CRUD-tól teljesen elkülönül
    // ============================================================
    template<typename Registry, typename Repository>
    StartupStatus initRegistryGeneric(const QString& errorMessage, bool critical = true)
    {
        using Entity = typename Registry::EntityType;

        QVector<Entity> items;
        bool ok = Repository::load(items);

        if (!ok)
            return StartupStatus::failure(errorMessage, critical);

        Registry::instance().bulkSetAll(items);

        zInfo(QString("📦 %1: %2 elem betöltve")
                  .arg(Registry::instance().name())
                  .arg(Registry::instance().size()));

        return StartupStatus::success();
    }

    // Speciális (nem generikus) lépések
    StartupStatus initRalColors();
    //StartupStatus initNeedRuleRegistry();

    //void MatrixValidator_2();
};
