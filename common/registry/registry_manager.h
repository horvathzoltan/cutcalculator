#pragma once
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
#include "common/model/barcode_identifiable_entity.h"
#include "registry_base.h"

#include <QVector>
#include <QString>

#include "common/utils/table_formatter.h"

class RegistryManager {
public:
    static RegistryManager& instance() {
        static RegistryManager mgr;
        return mgr;
    }

    // --- Egységes regisztráció ---
    void registerRepo(RegistryBase* repo) {
        repos.append(repo);
    }

    // --- Audit riport ---
    void auditReport() const {
        zInfo("📊 Registry audit összefoglaló:");

        QVector<QString> header = {"Registry", "Type", "Count"};
        QVector<QVector<QString>> rows;

        for (auto* repo : repos) {
            rows.push_back({
                repo->name(),
                repo->typeName(),
                QString::number(repo->size())
            });
        }

        const auto lines = TableFormatter::format(header, rows);
        for (const auto& line : lines)
            zInfo().noquote() << line;

        zInfo().noquote() << QString("Összesen: %1").arg(totalCount());
        zEvent(QString("Registry audit összefoglaló – összesen %1 elem tárolva").arg(totalCount()));
    }

    // --- Összes elemszám ---
    int totalCount() const {
        int sum = 0;
        for (auto* repo : repos)
            sum += repo->size();
        return sum;
    }

    // --- Keresés típusnév alapján ---
    RegistryBase* findByTypeName(const QString& typeName) const {
        for (auto* repo : repos)
            if (repo->typeName() == typeName)
                return repo;
        return nullptr;
    }

    // --- Entitás keresése ID alapján ---
    const BarcodeIdentifiableEntity* findEntity(const QString& typeName,
                                                const QUuid& id) const
    {
        if (auto* repo = findByTypeName(typeName))
            return repo->findEntityById(id);   // registry maga implementálja
        return nullptr;
    }

    const QVector<RegistryBase*>& allRepos() const {
        return repos;
    }

private:
    RegistryManager() = default;

    QVector<RegistryBase*> repos;
};

// // registry_manager.h
// #pragma once
// #include "common/logger/event_logger.h"
// #include "common/logger/logger.h"
// #include "common/model/barcode_identifiable_entity.h"
// //#include "barcode_identifiable_registry_base.h"
// #include "registry_base.h"

// #include <QVector>
// #include <QString>
// #include <QDebug>

// #include "common/utils/table_formatter.h"

// //class RegistryBase;

// /**
//  * @brief RegistryManager – singleton központi manager.
//  *
//  * Feladata:
//  *  - minden registry automatikus regisztrációja
//  *  - audit riport készítése
//  *  - összesített elemszám számítása
//  */
// class RegistryManager {
// public:
//     static RegistryManager& instance() {
//         static RegistryManager mgr;
//         return mgr;
//     }

//     void registerRepo(RegistryBase* repo) {
//         identifiable.append(repo);
//     }

//     // void registerRepo(RegistryBase* repo) {
//     //     repos.append(repo);
//     //     if (auto* idRepo = dynamic_cast<BarcodeIdentifiableRegistryBase*>(repo)) {
//     //         identifiable.append(idRepo);
//     //     }
//     // }

//     // registry_manager.cpp
//     void auditReport() const {
//         zInfo("📊 Registry audit összefoglaló:");

//         // --- Fejléc + sorok előkészítése ---
//         QVector<QString> header = {"Registry", "Type", "Count"};
//         QVector<QVector<QString>> rows;

//         for (auto* repo : repos) {
//             rows.push_back({
//                 repo->name(),
//                 repo->typeName(),
//                 QString::number(repo->size())
//             });
//         }

//         // --- Táblázat formázása ---
//         const auto lines = TableFormatter::format(header, rows);

//         for (const auto& line : lines)
//             zInfo().noquote() << line;

//         // --- Összesen sor ---
//         zInfo().noquote() << QString("Összesen: %1").arg(totalCount());

//         // Rövid nyugta zEvent-tel
//         zEvent(QString("Registry audit összefoglaló – összesen %1 elem tárolva").arg(totalCount()));
//     }



//     int totalCount() const {
//         int sum = 0;
//         for (auto* repo : repos) sum += repo->size();
//         return sum;
//     }

//     BarcodeIdentifiableRegistryBase* findByTypeName(const QString& typeName) const {
//         for (auto* repo : identifiable) {
//             if (repo->typeName() == typeName)
//                 return repo;
//         }
//         return nullptr;
//     }

//     const QVector<RegistryBase*>& allRepos() const {
//         return repos;
//     }

//     const BarcodeIdentifiableEntity* findEntity(const QString& typeName, const QUuid& id) const {
//         BarcodeIdentifiableRegistryBase* repo = findByTypeName(typeName);
//         if (repo) {
//             return repo->findEntityById(id); // minden registry implementálja → IdentifiableEntity*
//         }
//         return nullptr;
//     }

//     // registry_manager.h
//     // const IdentifiableEntity* findEntityByBarcode(const QString& typeName,
//     //                                               const QString& barcode) const {
//     //     IdentifiableRegistryBase* repo = findByTypeName(typeName);
//     //     if (repo) {
//     //         return repo->findEntityByBarcode(barcode); // minden registry implementálja
//     //     }
//     //     return nullptr;
//     // }


// private:
//     RegistryManager() = default;
//     QVector<RegistryBase*> repos;
//     //QVector<BarcodeIdentifiableRegistryBase*> identifiable;  // csak az azonosíthatók
// };
