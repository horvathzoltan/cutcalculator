// registry_manager.h
#pragma once
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
#include "registry_base.h"

#include <QVector>
#include <QString>
#include <QDebug>

//class RegistryBase;

/**
 * @brief RegistryManager – singleton központi manager.
 *
 * Feladata:
 *  - minden registry automatikus regisztrációja
 *  - audit riport készítése
 *  - összesített elemszám számítása
 */
class RegistryManager {
public:
    static RegistryManager& instance() {
        static RegistryManager mgr;
        return mgr;
    }

    void registerRepo(RegistryBase* repo) {
        repos.append(repo);
    }

    // registry_manager.cpp
    void auditReport() const {
        zInfo() << "📊 Registry audit összefoglaló:";
        zInfo().noquote() << QString("Registry           | Type              | Count");
        zInfo().noquote() << QString("-----------------------------------------------");

        for (auto* repo : repos) {
            zInfo().noquote() << QString("%1 | %2 | %3")
            .arg(repo->name(), -18)
                .arg(repo->typeName(), -18)
                .arg(repo->size(), 5);
        }

        zInfo().noquote() << QString("-----------------------------------------------");
        zInfo().noquote() << QString("Összesen: %1").arg(totalCount());

        // Rövid nyugta zEvent-tel
        zEvent(QString("Registry audit összefoglaló – összesen %1 elem tárolva").arg(totalCount()));
    }


    int totalCount() const {
        int sum = 0;
        for (auto* repo : repos) sum += repo->size();
        return sum;
    }

private:
    RegistryManager() = default;
    QVector<RegistryBase*> repos;
};
