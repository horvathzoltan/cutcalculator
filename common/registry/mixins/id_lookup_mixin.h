// common/registry/mixins/lookup_mixin.h
#pragma once

#include "../core/registry_core.h"
#include <functional>

template<typename E> concept HasIdField = requires(const E& e) { { e.id }; };

template<typename Host, typename Entity>
    requires HasIdField<Entity>
struct IdLookupMixin : RegistryCore<Host, Entity> {
    //LookupContractChecks<Host, Entity> _contract_check;

    QVector<Entity> readAll() const { return this->readAllImpl(); }

    const Entity* findById(const typename Entity::IdType& id) const {
        return this->findByIdImpl(id);
    }

    template<typename Predicate>
    QVector<Entity> findAll(Predicate&& pred) const {
        QVector<Entity> out;
        for (const auto& e : this->readAllImpl()) {
            if (pred(e)) out.append(e);
        }
        return out;
    }

    // subscribe helper (delegál a Host/RegistryEngineBase-nek)
    auto subscribeItemsChangedToken(std::function<void()> cb) {
        return static_cast<Host&>(*this).subscribeItemsChangedToken(std::move(cb));
    }

};
