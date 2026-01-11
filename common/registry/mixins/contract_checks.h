// common/registry/mixins/contract_checks.h
#pragma once

#include <concepts>
#include <QVector>

template<typename Host, typename Entity>
concept HasStoreApi = requires(Host h, const Entity& e, const typename Entity::IdType& id, const QVector<Entity>& v) {
    { h.storeAddImpl(e) } -> std::convertible_to<bool>;
    { h.storeAddAllImpl(v) } -> std::convertible_to<bool>;
    { h.storeReadAllImpl() } -> std::convertible_to<QVector<Entity>>;
    { h.storeFindByIdImpl(id) } -> std::convertible_to<const Entity*>;
    { h.storeUpdateImpl(e) } -> std::convertible_to<bool>;
    { h.storeRemoveImpl(id) } -> std::convertible_to<bool>;
};

template<typename Host, typename Entity>
struct CrudContractChecks {
    static_assert(HasStoreApi<Host, Entity>,
                  "Host must provide protected store*Impl methods for RegistryCore (storeAddImpl, storeFindByIdImpl, ...)");
};

template<typename Host, typename Entity>
concept HasCollectorStoreApi = requires(Host h, const Entity& e, const QVector<Entity>& v) {
    { h.storeAddImpl(e) } -> std::convertible_to<bool>;
    { h.storeAddAllImpl(v) } -> std::convertible_to<bool>;
    { h.storeReadAllImpl() } -> std::convertible_to<QVector<Entity>>;
};

template<typename Host, typename Entity>
struct CollectorContractChecks {
    static_assert(HasCollectorStoreApi<Host, Entity>,
                  "Host must provide storeAddImpl, storeAddAllImpl, storeReadAllImpl for CollectorMixin");
};



template<typename Host, typename Entity>
concept HasLookupPublicApi = requires(const Host h, const typename Entity::IdType& id) {
    { h.readAll() } -> std::convertible_to<QVector<Entity>>;
    { h.findById(id) } -> std::convertible_to<const Entity*>;
};

template<typename Host, typename Entity>
struct LookupContractChecks {
    static_assert(HasLookupPublicApi<Host, Entity>,
                  "Host must provide public readAll() and findById(IdType) for LookupMixin");
};
