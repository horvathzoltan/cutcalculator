// registry_manager.h
#pragma once
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
#include "common/model/barcode_identifiable_entity.h"
#include "common/registry/base/barcode_capable_interface.h"
#include "common/registry/feature/registry_base.h"
#include "common/registry/barcode/id_lookup_registry_interface.h"


#include <QVector>
#include <QString>
#include <QDebug>

#include "common/utils/table_formatter.h"

// template<typename T>
// concept BarcodeCapableRegistry =
//     requires(T t) {
//         // A BarcodeIndexMixin által biztosított API
//         { t.rebuildIndex() } -> std::same_as<void>;
//         { t.findByBarcode(QString{}) };
//     };

template<typename R>
concept HasFindById =
    requires(const R& r, const QUuid& id) {
        { r.findById(id) };
    };

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
    
    void validateRepoRegistration(RegistryBase* repo) {
        static QSet<QString> warned_duplicate;
        static QSet<QString> warned_typename;
        static QSet<QString> warned_barcode_capability;

        // --- Duplicate pointer ---
        for (auto* existing : repos) {
            if (existing == repo) {
                if (!warned_duplicate.contains(repo->name())) {
                    warned_duplicate.insert(repo->name());
                    zWarning().noquote()
                        << QString("⚠️ registerRepo: duplicate registry pointer detected: '%1'")
                               .arg(repo->name());
                }
                return;
            }

            // --- typeName collision ---
            if (existing->typeName() == repo->typeName()) {
                if (!warned_typename.contains(repo->typeName())) {
                    warned_typename.insert(repo->typeName());
                    zWarning().noquote()
                        << QString("⚠️ registerRepo: typeName collision: '%1'")
                               .arg(repo->typeName());
                }
            }
        }
    }

    void registerRepo(RegistryBase* repo) {
        validateRepoRegistration(repo);
        repos.append(repo);

        if (dynamic_cast<IBarcodeCapable*>(repo)) {
            identifiable.append(repo);
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

    RegistryBase* findByTypeName(const QString& typeName) const {
        static QSet<QString> warned_missing;
        static QSet<QString> warned_multiple;

        RegistryBase* found = nullptr;
        int matchCount = 0;

        for (auto* repo : repos) {   // <-- EZ A LÉNYEG
            if (repo->typeName() == typeName) {
                ++matchCount;
                if (!found)
                    found = repo;
                else if (!warned_multiple.contains(typeName)) {
                    warned_multiple.insert(typeName);
                    zWarning().noquote()
                        << QString("⚠️ findByTypeName: multiple registries found for typeName='%1'")
                               .arg(typeName);
                }
            }
        }

        if (!found && !warned_missing.contains(typeName)) {
            warned_missing.insert(typeName);
            zWarning().noquote()
                << QString("⚠️ findByTypeName: no registry found for typeName='%1'")
                       .arg(typeName);
        }

        return found;
    }

    const IdentifiableEntity* findEntity(const QString& typeName, const QUuid& id) const {
        const auto* repo = findByTypeName(typeName);
        if (!repo)
            return nullptr;

        const auto* idRepo = dynamic_cast<const IdLookupRegistryInterface*>(repo);
        if (!idRepo)
            return nullptr;

        return idRepo->findEntityById(id);
    }


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
    QVector<RegistryBase*> identifiable;   // csak a barcode-képes registryk audit és loggolás miatt.
    QVector<RegistryBase*> repos;          // minden registry
};


