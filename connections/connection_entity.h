#pragma once
#include "common/registry/manager/registry_traits.h"
//#include "connection_registry.h"

#include <QUuid>
#include <optional>
#include "common/registry/manager/registry_lookup.h"

/**
 * 🧩 ConnectionEntity – generikus kapcsolótábla sor
 *
 * Hunglish: ez a "kapcsos gyík" alapmodell. Bal és jobb oldali entitás GUID-ját tárolja,
 * és convenience lookupot ad típusból – nem piszkálja a repo/regit, csak lehívja, ha kell.
 *
 * Elvárás a LeftEntity/RightEntity típusoktól:
 *  - legyen static registry() vagy instance() metódusuk, amin findById(QUuid) elérhető
 *  - visszatérési érték std::optional<LeftEntity>/RightEntity (vagy pointer) – itt optionalt használunk
 */
template<typename LeftEntity, typename RightEntity>
struct ConnectionEntity {
    QUuid leftId{};
    QUuid rightId{};

    // Hunglish: lookup – bal oldali entitás visszafejtése GUID-ból
    std::optional<LeftEntity> left() const {
        using RegistryType = typename RegistryFor<LeftEntity>::type;
        if (auto* reg = lookupRegistry<RegistryType>()) {
            if (auto* found = reg->findById(leftId))
                return *found;
        }
        return std::nullopt;
    }

    // Hunglish: lookup – jobb oldali entitás visszafejtése GUID-ból
    // std::optional<RightEntity> right() const {
    //     if (auto opt = RightEntity::registry().findById(rightId)) {
    //         return opt;
    //     }
    //     return std::nullopt;
    // }

    std::optional<RightEntity> right() const {
        using RegistryType = typename RegistryFor<RightEntity>::type;
        if (auto* reg = lookupRegistry<RegistryType>()) {
            if (auto* found = reg->findById(rightId))
                return *found;
        }
        return std::nullopt;
    }

    QString leftBarcode() const {
        if (auto opt = left()) {
            return opt->barcode;   // mert LeftEntity örökli az IdentifiableEntity-t
        }
        return {};
    }

    QString rightBarcode() const {
        if (auto opt = right()) {
            return opt->barcode;   // mert RightEntity örökli az IdentifiableEntity-t
        }
        return {};
    }



    // Audit convenience: emberbarát rövid leírás (ha van barcode/name)
    QString describe() const {
        QString l = leftId.toString(QUuid::WithBraces);
        QString r = rightId.toString(QUuid::WithBraces);
        return QString("Connection{ left=%1, right=%2 }").arg(l, r);
    }
};
