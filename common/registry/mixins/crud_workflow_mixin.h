#pragma once

#include "common/registry/mixins/crud_mixin.h"
template<typename Host, typename Entity>
struct CrudWorkflowMixin {
    using IdType = typename Entity::IdType;

    bool insertWithWorkflow(Entity e) {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.validateDomain(e)) return false;
        if (!reg.validateDuplicate(e)) return false;
        if (!reg.beforeInsert(e)) return false;

        // --- Tényleges beszúrás: kifejezetten a CrudMixin insert() metódusa ---
        if (!static_cast<CrudMixin<Host, Entity>&>(reg).insert(e))
            return false;

        reg.onInsertLog(e);
        reg.persist();
        return true;
    }

    bool updateWithWorkflow(Entity e) {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.validateDomain(e)) return false;
        if (!reg.validateDuplicate(e)) return false;
        if (!reg.beforeUpdate(e)) return false;

        // --- Tényleges update: kifejezetten a CrudMixin update() ---
        if (!static_cast<CrudMixin<Host, Entity>&>(reg).update(e))
            return false;

        reg.onUpdateLog(e);
        reg.persist();
        return true;
    }

    bool removeWithWorkflow(const IdType& id) {
        Host& reg = static_cast<Host&>(*this);

        const Entity* existing = reg.findById(id);
        if (!existing) return false;

        Entity copy = *existing;

        if (!reg.beforeRemove(copy)) return false;

        // --- Tényleges remove: kifejezetten a CrudMixin remove() ---
        if (!static_cast<CrudMixin<Host, Entity>&>(reg).remove(id))
            return false;

        reg.onRemoveLog(copy);
        reg.persist();
        return true;
    }
};
