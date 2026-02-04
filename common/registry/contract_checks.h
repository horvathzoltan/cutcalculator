#pragma once
#include "common/registry/mixins/mixin_contract_checks.h" // itt vannak a Has... trait-ek

#define REGISTER_LOOKUP_CHECK(Host, Entity) \
    static_assert(HasLookupPublicApi<Host, Entity>, \
                  #Host " must provide public readAll() and findById(IdType) for LookupMixin");

#define REGISTER_CRUD_CHECK(Host, Entity) \
    static_assert(HasCrudPublicApi<Host, Entity>, \
                  #Host " must provide insert/update/remove/readAll/findById for CrudMixin");

#define REGISTER_BARCODE_INDEX_CHECK(Host, Entity) \
    static_assert(HasBarcodeIndexApi<Host, Entity>, \
                  #Host " must provide barcode index API required by BarcodeIndexMixin");

#define REGISTER_REGISTERME_CHECK(Host) \
    static_assert(HasRegisterMeApi<Host>, \
                  #Host " must satisfy RegisterMe contract (static registration entry point)");

#define REGISTER_PERSIST_CHECK(Host) \
    static_assert(HasPersistApi<Host>, \
                  #Host " must provide persist() and persistRegistry() if required");

#define REGISTER_CRUD_WORKFLOW_CHECK(Host, Entity) \
    static_assert(HasCrudWorkflowApi<Host, Entity>::value, \
                  #Host " must satisfy the CrudWorkflowMixin contract")

#define REGISTER_CONNECTION_WORKFLOW_CHECK(Host, Connection) \
    static_assert(HasConnectionWorkflowApi<Host, Connection>::value, \
                  #Host " must satisfy the ConnectionWorkflowMixin contract")
