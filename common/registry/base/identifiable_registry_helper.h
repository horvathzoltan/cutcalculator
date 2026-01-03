#pragma once

#include "identifiable_registry_engine.h"

//#include "barcodes/registry/identifiable_registry_engine.h"
//#include "common/model/identifiable_entity.h"
//#include "common/logger/logger.h"

struct IdentifiableRegistryHelper{
    //template<typename RegistryT, typename EntityT>
    //static bool insert(RegistryT& registry, const EntityT& e)
    template<typename EntityT>
    static bool insert(IdentifiableRegistryEngine<EntityT>& registry, const EntityT& e)

    {
        // 1) Domain validáció
        if (!registry.validateDomain(e))
            return false;

        // 2) Duplikáció ellenőrzés
        if (!registry.validateDuplicate(e))
            return false;

        // 3) Before hook
        if (!registry.beforeInsert(e))
            return false;

        // 4) CRUD add
        registry.add(e);

        // 5) After hook
        registry.afterInsert(e);

        // 6) Log hook
        registry.onInsertLog(e);

        // 7) persist
        registry.persist();

        return true;
    }

    //template<typename RegistryT>
    //static bool remove(RegistryT& registry, const QUuid& id)
    template<typename EntityT>
    static bool remove(IdentifiableRegistryEngine<EntityT>& registry, const QUuid& id)
    {
        //using EntityT = typename RegistryT::entity_type;

        // 1) find entity
        const EntityT* found = registry.findById(id);
        if (!found)
            return false;

        EntityT e = *found; // copy for logging

        // 2) before hook
        if (!registry.beforeRemove(e))
            return false;

        // 3) remove
        if (!registry.removeInternal(id))
            return false;

        // 4) after hook
        registry.afterRemove(e);

        // 5) log
        registry.onRemoveLog(e);

        // 6) persist
        registry.persist();

        return true;
    }

    template<typename EntityT>
    static bool update(IdentifiableRegistryEngine<EntityT>& registry, const EntityT& e)
    {
        // 1) before hook
        if (!registry.beforeUpdate(e))
            return false;

        // 2) raw update
        if (!registry.updateInternal(e))
            return false;

        // 3) after hook
        registry.afterUpdate(e);

        // 4) log
        registry.onUpdateLog(e);

        // 5) persist
        registry.persist();

        return true;
    }

};
