// common/registry/mixins/collector_mixin.h
#pragma once

#include "../core/registry_core.h"
#include "contract_checks.h"
#include <QString>

template<typename Host, typename Entity>
struct CollectorMixin : RegistryCore<Host, Entity> {
    CollectorContractChecks<Host, Entity> _contract_check;

    bool add(const Entity& e) {
        Host& reg = static_cast<Host&>(*this);
        Entity mutableCopy = e;
        if (!reg.beforeInsert(mutableCopy)) return false;
        if (!this->addImpl(mutableCopy)) return false;
        reg.afterInsert(mutableCopy);
        reg.onInsertLog(mutableCopy);
        reg.persistRegistry();
        reg.notifyItemsChanged();
        return true;
    }

    bool addAll(const QVector<Entity>& v) {
        Host& reg = static_cast<Host&>(*this);
        if (!this->addAllImpl(v)) return false;
        reg.persistRegistry();
        reg.notifyItemsChanged();
        return true;
    }

    // Optional: host can override importCsvImpl for custom parsing
    bool importCsv(const QString& path) {
        Host& reg = static_cast<Host&>(*this);
        return reg.importCsvImpl(path);
    }

    // read helpers delegated to core
    QVector<Entity> readAll() const { return this->readAllImpl(); }
    const Entity* findById(const typename Entity::IdType& id) const { return this->findByIdImpl(id); }
};
