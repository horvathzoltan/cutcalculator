// registry_manager.h
#pragma once
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
#include "common/model/barcode_identifiable_entity.h"
#include "common/registry/feature/registry_base.h"
#include "common/registry/barcode/ibarcode_identifiable_registry_base.h"


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

    // void registerRepo(RegistryBase* repo) {
    //     identifiable.append(repo);
    // }

    void validateRepoRegistration(RegistryBase* repo) {
        static bool warned_duplicate = false;
        static bool warned_typename = false;
        static bool warned_cast = false;

        for (auto* existing : repos) {
            if (existing == repo) {
                if (!warned_duplicate) {
                    warned_duplicate = true;
                    zWarning().noquote() << QString("⚠️ registerRepo: duplicate registry pointer detected: '%1'").arg(repo->name());
                }
                return;
            }
            if (existing->typeName() == repo->typeName()) {
                if (!warned_typename) {
                    warned_typename = true;
                    zWarning().noquote() << QString("⚠️ registerRepo: typeName collision: '%1'").arg(repo->typeName());
                }
            }
        }

        if (!dynamic_cast<IBarcodeIdentifiableRegistryBase*>(repo)) {
            if (!warned_cast) {
                warned_cast = true;
                zWarning().noquote() << QString("⚠️ registerRepo: registry '%1' is not barcode-identifiable").arg(repo->name());
            }
        }
    }

    void registerRepo(RegistryBase* repo) {
        validateRepoRegistration(repo);
        repos.append(repo);
        if (auto* idRepo = dynamic_cast<IBarcodeIdentifiableRegistryBase*>(repo)) {
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
        const auto lines = TableFormatter::format(header, rows);

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

    IBarcodeIdentifiableRegistryBase* findByTypeName(const QString& typeName) const {
        static QSet<QString> warned_missing;
        static QSet<QString> warned_multiple;
        static QSet<QString> warned_cast;

        IBarcodeIdentifiableRegistryBase* found = nullptr;
        int matchCount = 0;

        for (auto* repo : identifiable) {
            if (repo->typeName() == typeName) {
                ++matchCount;
                if (!found)
                    found = repo;
                else if (!warned_multiple.contains(typeName)) {
                    warned_multiple.insert(typeName);
                    zWarning().noquote() << QString("⚠️ findByTypeName: multiple identifiable registries found for typeName='%1'").arg(typeName);
                }
            }
        }

        if (!found && !warned_missing.contains(typeName)) {
            warned_missing.insert(typeName);
            zWarning().noquote() << QString("⚠️ findByTypeName: identifiable registry not found for typeName='%1'").arg(typeName);
        }


        if (found && !dynamic_cast<IBarcodeIdentifiableRegistryBase*>(found) && !warned_cast.contains(typeName)) {
            warned_cast.insert(typeName);
            zWarning().noquote() << QString("⚠️ findByTypeName: typeName match, but dynamic_cast failed for '%1'").arg(typeName);
        }


        return found;
    }


    // IBarcodeIdentifiableRegistryBase* findByTypeName(const QString& typeName) const {
    //     for (auto* repo : identifiable) {
    //         if (repo->typeName() == typeName)
    //             return repo;
    //     }
    //     return nullptr;
    // }

    // const QVector<RegistryBase*>& allRepos() const {
    //     return repos;
    // }

    // const BarcodeIdentifiableEntity* findEntity(const QString& typeName, const QUuid& id) const {
    //     auto* repo = findByTypeName(typeName);
    //     if (repo) {
    //         return repo->findEntityById(id); // minden registry implementálja → IdentifiableEntity*
    //     }
    //     return nullptr;
    // }

    const BarcodeIdentifiableEntity* findEntity(const QString& typeName, const QUuid& id) const {
        static QSet<QString> warned_missing_id;

        auto* repo = findByTypeName(typeName);
        if (!repo)
            return nullptr;

        const BarcodeIdentifiableEntity* entity = repo->findEntityById(id);
        if (!entity && !warned_missing_id.contains(typeName)) {
            warned_missing_id.insert(typeName);
            zWarning().noquote() << QString("⚠️ findEntity: entity not found in registry='%1', id='%2'")
                                        .arg(typeName, id.toString(QUuid::WithoutBraces));
        }

        return entity;
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

    template<typename RegistryType>
    RegistryType* lookupRegistry() {
        static QSet<QString> warned_missing;
        static QSet<QString> warned_cast;
        static QSet<QString> warned_multiple;

        const QString expectedType = RegistryType::instance().typeName();
        RegistryType* found = nullptr;
        int matchCount = 0;
        //RegistryManager::instance().allRepos()
        for (auto* repo : repos) {
            if (repo->typeName() == expectedType) {
                ++matchCount;
                if (auto* casted = dynamic_cast<RegistryType*>(repo)) {
                    if (!found)
                        found = casted;
                } else {
                    if (!warned_cast.contains(expectedType)) {
                        warned_cast.insert(expectedType);
                        zWarning().noquote() << QString("⚠️ lookupRegistry: typeName match, but dynamic_cast failed. expected='%1', repo='%2'")
                                                    .arg(expectedType, repo->name());
                    }
                }
            }
        }

        if (matchCount > 1 && !warned_multiple.contains(expectedType)) {
            warned_multiple.insert(expectedType);
            zWarning().noquote() << QString("⚠️ lookupRegistry: multiple registries found for typeName='%1' (%2 matches)")
                                        .arg(expectedType).arg(matchCount);
        }

        if (!found && !warned_missing.contains(expectedType)) {
            warned_missing.insert(expectedType);
            zWarning().noquote() << QString("⚠️ lookupRegistry: registry not found for typeName='%1'. Did you forget inst.initialize()?").arg(expectedType);
        }


        return found;
    }
private:
    RegistryManager() = default;
    QVector<RegistryBase*> repos;
    QVector<IBarcodeIdentifiableRegistryBase*> identifiable;  // csak az azonosíthatók
};

// #pragma once
// #include "common/logger/event_logger.h"
// #include "common/logger/logger.h"
// #include "common/model/barcode_identifiable_entity.h"
// #include "registry_base.h"

// #include <QVector>
// #include <QString>

// #include "common/utils/table_formatter.h"

// class RegistryManager {
// public:
//     static RegistryManager& instance() {
//         static RegistryManager mgr;
//         return mgr;
//     }

//     // --- Egységes regisztráció ---
//     void registerRepo(RegistryBase* repo) {
//         repos.append(repo);
//     }

//     // --- Audit riport ---
//     void auditReport() const {
//         zInfo("📊 Registry audit összefoglaló:");

//         QVector<QString> header = {"Registry", "Type", "Count"};
//         QVector<QVector<QString>> rows;

//         for (auto* repo : repos) {
//             rows.push_back({
//                 repo->name(),
//                 repo->typeName(),
//                 QString::number(repo->size())
//             });
//         }

//         const auto lines = TableFormatter::format(header, rows);
//         for (const auto& line : lines)
//             zInfo().noquote() << line;

//         zInfo().noquote() << QString("Összesen: %1").arg(totalCount());
//         zEvent(QString("Registry audit összefoglaló – összesen %1 elem tárolva").arg(totalCount()));
//     }

//     // --- Összes elemszám ---
//     int totalCount() const {
//         int sum = 0;
//         for (auto* repo : repos)
//             sum += repo->size();
//         return sum;
//     }

//     // --- Keresés típusnév alapján ---
//     RegistryBase* findByTypeName(const QString& typeName) const {
//         for (auto* repo : repos)
//             if (repo->typeName() == typeName)
//                 return repo;
//         return nullptr;
//     }

//     // --- Entitás keresése ID alapján ---
//     const BarcodeIdentifiableEntity* findEntity(const QString& typeName,
//                                                 const QUuid& id) const
//     {
//         if (auto* repo = findByTypeName(typeName))
//             return repo->findEntityById(id);   // registry maga implementálja
//         return nullptr;
//     }

//     const QVector<RegistryBase*>& allRepos() const {
//         return repos;
//     }

// private:
//     RegistryManager() = default;

//     QVector<RegistryBase*> repos;
// };

