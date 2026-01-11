#pragma once

template<typename Host, typename Entity>
struct CrudWorkflowMixin {
    using IdType = typename Entity::IdType;

    bool insertWithWorkflow(Entity e) {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.validateDomain(e)) return false;
        if (!reg.validateDuplicate(e)) return false;
        if (!reg.beforeInsert(e)) return false;

        // --- Tényleges beszúrás: a CrudMixin insert() metódusa ---
        if (!reg.insert(e)) return false;

        reg.onInsertLog(e);
        reg.persist();
        return true;
    }

    bool updateWithWorkflow(Entity e) {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.validateDomain(e)) return false;
        if (!reg.beforeUpdate(e)) return false;

        // --- Tényleges update: CrudMixin update() ---
        if (!reg.update(e)) return false;

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

        // --- Tényleges remove: CrudMixin remove() ---
        if (!reg.remove(id)) return false;

        reg.onRemoveLog(copy);
        reg.persist();
        return true;
    }
};
