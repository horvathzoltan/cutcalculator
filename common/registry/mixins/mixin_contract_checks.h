// path: common/registry/mixins/contract_checks.h
#pragma once

#include <concepts>
#include <QVector>

// ============================================================
// 1) RegistryCore szerződés (store*Impl API)
// ============================================================

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
                  "Host must provide protected store*Impl methods for RegistryCore");
};

// ============================================================
// 2) CrudWorkflowMixin szerződés
//    (FIGYELEM: itt NEM ellenőrzünk protected store*Impl-et!)
// ============================================================

template<typename Host, typename Entity, typename = void>
struct HasCrudWorkflowApi : std::false_type {};

template<typename Host, typename Entity>
struct HasCrudWorkflowApi<Host, Entity, std::void_t<
                                            // validate* – const Host, const Entity&
                                            decltype(std::declval<const Host&>().validateDomain(std::declval<const Entity&>())),
                                            decltype(std::declval<const Host&>().validateDuplicate(std::declval<const Entity&>())),
                                            // before* – nem-const Host, nem-const Entity&
                                            decltype(std::declval<Host&>().beforeInsert(std::declval<Entity&>())),
                                            decltype(std::declval<Host&>().beforeUpdate(std::declval<Entity&>())),
                                            decltype(std::declval<Host&>().beforeRemove(std::declval<Entity&>())),
                                            // log – nem-const Host, const Entity&
                                            decltype(std::declval<Host&>().onInsertLog(std::declval<const Entity&>())),
                                            decltype(std::declval<Host&>().onUpdateLog(std::declval<const Entity&>())),
                                            decltype(std::declval<Host&>().onRemoveLog(std::declval<const Entity&>())),
                                            // persist – const Host
                                            decltype(std::declval<const Host&>().persist())
                                            >> : std::true_type {};

template<typename Host, typename Entity>
struct CollectorContractChecks {
    static_assert(
        HasCrudWorkflowApi<Host, Entity>::value,
        "Host must satisfy the CrudWorkflowMixin contract"
        );
};

// ============================================================
// 3) LookupMixin szerződés
// ============================================================

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

// ============================================================
// 4) ConnectionWorkflowMixin szerződés
//    (itt SEM ellenőrzünk protected store*Impl/notify-t)
// ============================================================

template<typename Host, typename Connection, typename = void>
struct HasConnectionWorkflowApi : std::false_type {};

template<typename Host, typename Connection>
struct HasConnectionWorkflowApi<Host, Connection, std::void_t<
                                                      // validate – const Host, const Connection&
                                                      decltype(std::declval<const Host&>().validateConnection(std::declval<const Connection&>())),
                                                      decltype(std::declval<const Host&>().validateDuplicate(std::declval<const Connection&>())),
                                                      // log – nem-const Host, const Connection&
                                                      decltype(std::declval<Host&>().onInsertLog(std::declval<const Connection&>())),
                                                      decltype(std::declval<Host&>().onRemoveLog(std::declval<const Connection&>())),
                                                      // persist – const Host
                                                      decltype(std::declval<const Host&>().persist()),
                                                      // lookup – const Host, IdType, IdType
                                                      decltype(std::declval<const Host&>().findByPair(
                                                          std::declval<typename Connection::IdType>(),
                                                          std::declval<typename Connection::IdType>()
                                                          ))
                                                      >> : std::true_type {};
