#pragma once

#include "common/registry/workflow/base_workflow_policy.h"

#include <QUuid>

// 🧩 LookupWorkflowPolicy
// - read-only registryk workflow-ja (ColorRegistry, stb.)
// - BaseWorkflowPolicy-ból származik
// - nincs insert/update/remove
// - csak load + index építés + persist

struct LookupWorkflowPolicy : BaseWorkflowPolicy
{
    // LOAD (pl. CSV-ből, JSON-ből, stb.)
    template<typename Registry, typename Container>
    static bool load(Registry& reg, const Container& items)
    {
        return run("lookup-load", [&]{
            beforeOperation(reg, "lookup-load");

            // 1) Adatok beállítása
            reg.setAll(items);

            // 2) Index építés (ha van)
            if constexpr (requires(Registry r) { r.buildIndex(); }) {
                reg.buildIndex();
            }

            // 3) Log (ha van)
            if constexpr (requires(Registry r) { r.onLoadLog(); }) {
                reg.onLoadLog();
            }

            // 4) Persist (ha van)
            persist(reg);

            afterOperation(reg, "lookup-load");
            return true;
        });
    }

    // INSERT → nem támogatott
    template<typename Registry, typename Entity>
    static bool insert(Registry&, const Entity&)
    {
        static_assert(!std::is_same_v<Registry, Registry>,
                      "LookupWorkflowPolicy: insert() not supported.");
        return false;
    }

    // UPDATE → nem támogatott
    template<typename Registry, typename Entity>
    static bool update(Registry&, const Entity&)
    {
        static_assert(!std::is_same_v<Registry, Registry>,
                      "LookupWorkflowPolicy: update() not supported.");
        return false;
    }

    // REMOVE → nem támogatott
    template<typename Registry>
    static bool remove(Registry&, const QUuid&)
    {
        static_assert(!std::is_same_v<Registry, Registry>,
                      "LookupWorkflowPolicy: remove() not supported.");
        return false;
    }
};
