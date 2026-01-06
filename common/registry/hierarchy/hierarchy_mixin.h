#pragma once

#include <QVector>
#include <QUuid>

/**
 * 🧩 HierarchyMixin<Derived, Entity>
 *
 * Feltételek:
 *  - Derived örököl RegistryEngineBase<Entity>-ből (vagy kompatibilis basisból)
 *  - Entity rendelkezik `parentId` mezővel (QUuid)
 *  - Derived elérhetővé teszi a `findIf` és `readAll` metódusokat
 *
 * Eredmény:
 *  - findChildren(parentId)
 *  - findRoots()
 */
template<typename Derived, typename Entity>
class HierarchyMixin {
public:
    QVector<Entity> findChildren(const QUuid& parentId) const
    {
        auto* self = static_cast<const Derived*>(this);
        return self->findAll([&](const Entity& e){
            return e.parentId == parentId;
        });
    }

    QVector<Entity> findRoots() const
    {
        auto* self = static_cast<const Derived*>(this);
        return self->findAll([&](const Entity& e){
            return e.parentId.isNull();
        });
    }
};
