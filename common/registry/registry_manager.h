// registry_manager.h
#pragma once
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
#include "common/model/identifiable_entity.h"
#include "registry_base.h"

#include <QVector>
#include <QString>
#include <QDebug>

#include "common/utils/table_formatter.h"

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

    void registerRepo(IdentifiableRegistryBase* repo) {
        identifiable.append(repo);
    }

    void registerRepo(RegistryBase* repo) {
        repos.append(repo);
        if (auto* idRepo = dynamic_cast<IdentifiableRegistryBase*>(repo)) {
            identifiable.append(idRepo);
        }
    }

    // registry_manager.cpp
    void auditReport() const {
        zInfo("📊 Registry audit összefoglaló:");

        // --- Fejléc + sorok előkészítése ---
        QVector<QString> header = {"Registry", "Type", "Count"};
        QVector<QVector<QString>> rows;

        for (auto* repo : repos) {
            rows.push_back({
                repo->name(),
                repo->typeName(),
                QString::number(repo->size())
            });
        }

        // --- Táblázat formázása ---
        auto lines = TableFormatter::format(header, rows);

        for (const auto& line : lines)
            zInfo().noquote() << line;

        // --- Összesen sor ---
        zInfo().noquote() << QString("Összesen: %1").arg(totalCount());

        // Rövid nyugta zEvent-tel
        zEvent(QString("Registry audit összefoglaló – összesen %1 elem tárolva").arg(totalCount()));
    }



    int totalCount() const {
        int sum = 0;
        for (auto* repo : repos) sum += repo->size();
        return sum;
    }

    IdentifiableRegistryBase* findByTypeName(const QString& typeName) const {
        for (auto* repo : identifiable) {
            if (repo->typeName() == typeName)
                return repo;
        }
        return nullptr;
    }

    const QVector<RegistryBase*>& allRepos() const {
        return repos;
    }

    const IdentifiableEntity* findEntity(const QString& typeName, const QUuid& id) const {
        IdentifiableRegistryBase* repo = findByTypeName(typeName);
        if (repo) {
            return repo->findEntityById(id); // minden registry implementálja → IdentifiableEntity*
        }
        return nullptr;
    }

    // registry_manager.h
    // const IdentifiableEntity* findEntityByBarcode(const QString& typeName,
    //                                               const QString& barcode) const {
    //     IdentifiableRegistryBase* repo = findByTypeName(typeName);
    //     if (repo) {
    //         return repo->findEntityByBarcode(barcode); // minden registry implementálja
    //     }
    //     return nullptr;
    // }


private:
    RegistryManager() = default;
    QVector<RegistryBase*> repos;
    QVector<IdentifiableRegistryBase*> identifiable;  // csak az azonosíthatók
};
