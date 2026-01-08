#pragma once

#include "common/registry/workflow/base_workflow_policy.h"

#include <QUuid>

// 🧩 CrudWorkflowPolicy
// - minden CRUD-alapú registry workflow-ja
// - NEM tud ID-ről, NEM tud barcode-ról
// - csak a workflow sorrendet adja
// - a domain logika a registry engine hookjaiban van

struct CrudWorkflowPolicy : BaseWorkflowPolicy
{
    // INSERT
    template<typename Registry, typename Entity>
    static bool insert(Registry& reg, const Entity& e)
    {
        return run("insert", [&]{
            beforeOperation(reg, "insert");

            // Domain validációk
            if (!reg.validateDomain(e))    return false;
            if (!reg.validateDuplicate(e)) return false;

            // Before hook
            if (!reg.beforeInsert(e))      return false;

            // CRUD
            reg.add(e);

            // After hook
            reg.afterInsert(e);

            // Log hook
            reg.onInsertLog(e);

            // Persist
            persist(reg);

            afterOperation(reg, "insert");
            return true;
        });
    }

    // UPDATE
    template<typename Registry, typename Entity>
    static bool update(Registry& reg, const Entity& e)
    {
        return run("update", [&]{
            beforeOperation(reg, "update");

            // Before hook
            if (!reg.beforeUpdate(e)) return false;

            // CRUD
            if (!reg.updateInternal(e)) return false;

            // After hook
            reg.afterUpdate(e);

            // Log hook
            reg.onUpdateLog(e);

            // Persist
            persist(reg);

            afterOperation(reg, "update");
            return true;
        });
    }

    // REMOVE
    template<typename Registry>
    static bool remove(Registry& reg, const QUuid& id)
    {
        return run("remove", [&]{
            beforeOperation(reg, "remove");

            using Entity = typename Registry::EntityType;

            // Find entity
            const Entity* found = reg.findById(id);
            if (!found)
                return false;

            Entity copy = *found; // logoláshoz

            // Before hook
            if (!reg.beforeRemove(copy)) return false;

            // CRUD
            if (!reg.removeInternal(id)) return false;

            // After hook
            reg.afterRemove(copy);

            // Log hook
            reg.onRemoveLog(copy);

            // Persist
            persist(reg);

            afterOperation(reg, "remove");
            return true;
        });
    }


    // template<typename Registry>
    // static bool remove(Registry& reg, const QUuid& leftId, const QUuid& rightId)
    // {
    //     return run("remove-pair", [&]{
    //         beforeOperation(reg, "remove-pair");

    //         if (!reg.beforeRemovePair(leftId, rightId))
    //             return false;

    //         if (!reg.removeInternal(leftId, rightId))
    //             return false;

    //         reg.afterRemovePair(leftId, rightId);
    //         reg.onRemovePairLog(leftId, rightId);

    //         persist(reg);
    //         afterOperation(reg, "remove-pair");
    //         return true;
    //     });
    // }

};
