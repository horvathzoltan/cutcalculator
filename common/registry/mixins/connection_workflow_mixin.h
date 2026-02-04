#pragma once

//#include "mixin_contract_checks.h"


template<typename Host, typename Connection>
struct ConnectionWorkflowMixin {
    // static_assert(
    //     HasConnectionWorkflowApi<Host, Connection>::value,
    //     "Host must satisfy the ConnectionWorkflowMixin contract"
    //     );

    using IdType = typename Connection::IdType;

    bool insertWithWorkflow(const IdType& leftId, const IdType& rightId) {
        Host& reg = static_cast<Host&>(*this);
        Connection c(leftId, rightId);

        if (!reg.validateConnection(c)) return false;
        if (!reg.validateDuplicate(c)) return false;

        if (!reg.insertRaw(c)) return false;

        reg.onInsertLog(c);
        reg.persist();
        return true;
    }

    bool removeWithWorkflow(const IdType& leftId, const IdType& rightId) {
        Host& reg = static_cast<Host&>(*this);

        const Connection* existing = reg.findByPair(leftId, rightId);
        if (!existing)
            return false;

        Connection copy = *existing;

        if (!reg.removeRaw(leftId, rightId)) return false;

        reg.onRemoveLog(copy);
        reg.persist();
        return true;
    }
};

// template<typename Host, typename Connection>
// struct ConnectionWorkflowMixin {
//     using IdType = typename Connection::IdType;

//     bool insertWithWorkflow(const IdType& leftId, const IdType& rightId) {
//         Host& reg = static_cast<Host&>(*this);

//         Connection c(leftId, rightId);

//         // --- Domain validation ---
//         if (!reg.validateConnection(c)) return false;
//         if (!reg.validateDuplicate(c)) return false;
//         // ha lesz:
//         // if (!reg.beforeInsert(c)) return false;

//         // --- Tényleges beszúrás (ConnectionCrudMixin::insert) ---
//         if (!reg.insert(c)) return false;

//         // --- Domain log + persist ---
//         reg.onInsertLog(c);
//         reg.persist();
//         return true;
//     }

//     bool removeWithWorkflow(const IdType& leftId, const IdType& rightId) {
//         Host& reg = static_cast<Host&>(*this);

//         using C = Connection;
//         const C* existing = reg.findByPair(leftId, rightId);
//         if (!existing)
//             return false;

//         C copy = *existing;

//         // ha lesz:
//         // if (!reg.beforeRemove(copy)) return false;

//         // --- Tényleges törlés (ConnectionCrudMixin::remove) ---
//         if (!reg.remove(leftId, rightId)) return false;

//         // --- Domain log + persist ---
//         reg.onRemoveLog(copy);
//         reg.persist();
//         return true;
//     }
// };
