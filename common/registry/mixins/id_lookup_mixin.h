#pragma once

//#include "../core/registry_core.h"
#include <QVector>
#include <functional>

template<typename E> concept HasIdField = requires(const E& e) { { e.id }; };

template<typename Host, typename Entity>
    requires HasIdField<Entity>
struct IdLookupMixin {

    // Delegálás a Host (RegistryEngineBase) read API-jára
    QVector<Entity> readAll() const {
        return static_cast<const Host*>(this)->readAll();
    }

    const Entity* findById(const typename Entity::IdType& id) const {
        return static_cast<const Host*>(this)->findById(id);
    }

    template<typename Predicate>
    QVector<Entity> findAll(Predicate&& pred) const {
        return static_cast<const Host*>(this)->findAll(std::forward<Predicate>(pred));
    }

    auto subscribeItemsChangedToken(std::function<void()> cb) {
        return static_cast<Host&>(*this).subscribeItemsChangedToken(std::move(cb));
    }
};
