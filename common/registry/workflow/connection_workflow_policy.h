#pragma once

#include "common/registry/workflow/crud_workflow_policy.h"

// 🧩 ConnectionWorkflowPolicy
// - kapcsolattáblák workflow-ja (NeedRule, NeedCalcDetail, stb.)
// - CRUD workflow-ból származik
// - nincs update
// - insert/remove specializált domain validációt igényel
// - a domain logika a registry engine hookjaiban van

struct ConnectionWorkflowPolicy : CrudWorkflowPolicy
{
    // INSERT kapcsolat
    template<typename Registry, typename Connection>
    static bool insert(Registry& reg, const Connection& c)
    {
        return run("connect-insert", [&]{
            beforeOperation(reg, "connect-insert");

            // Domain validációk (kapcsolat-specifikus)
            if (!reg.validateConnection(c)) return false;
            if (!reg.validateDuplicate(c))  return false;

            // Before hook
            if (!reg.beforeInsert(c)) return false;

            // CRUD
            reg.add(c);

            // After hook
            reg.afterInsert(c);

            // Log hook
            reg.onInsertLog(c);

            // Persist
            persist(reg);

            afterOperation(reg, "connect-insert");
            return true;
        });
    }

    // REMOVE kapcsolat
    template<typename Registry>
    static bool remove(Registry& reg, const QUuid& leftId, const QUuid& rightId)
    {
        return run("connect-remove", [&]{
            beforeOperation(reg, "connect-remove");

            using Connection = typename Registry::EntityType;

            // Find entity by composite key
            const Connection* found = reg.findByPair(leftId, rightId);
            if (!found)
                return false;

            Connection copy = *found;

            // Before hook
            if (!reg.beforeRemove(copy)) return false;

            // CRUD
            if (!reg.removeInternal(leftId, rightId)) return false;

            // After hook
            reg.afterRemove(copy);

            // Log hook
            reg.onRemoveLog(copy);

            // Persist
            persist(reg);

            afterOperation(reg, "connect-remove");
            return true;
        });
    }

    // UPDATE → kapcsolattáblákban nincs értelme
    template<typename Registry, typename Connection>
    static bool update(Registry&, const Connection&)
    {
        static_assert(!std::is_same_v<Registry, Registry>,
                      "ConnectionWorkflowPolicy: update() not supported.");
        return false;
    }
};
