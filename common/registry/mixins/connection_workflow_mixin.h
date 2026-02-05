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
        if (!reg.beforeInsert(c)) return false;

        if (!reg.storeAddImpl(c)) return false;
        reg.notifyItemsChanged();

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
        if (!reg.beforeRemove(copy)) return false;

        if (!reg.storeRemovePairImpl(leftId, rightId)) return false;
        reg.notifyItemsChanged();

        reg.onRemoveLog(copy);
        reg.persist();
        return true;
    }

    bool replaceWithWorkflow(const IdType& leftId,
                             const IdType& oldRightId,
                             const IdType& newRightId)
    {
        Host& reg = static_cast<Host&>(*this);

        // --- Find old connection ---
        const Connection* existing = reg.findByPair(leftId, oldRightId);
        if (!existing)
            return false;

        Connection oldCopy = *existing;
        Connection newConn(leftId, newRightId);

        if (!reg.validateConnection(newConn)) return false;
        if (!reg.validateDuplicate(newConn)) return false;

        if (!reg.beforeRemove(oldCopy)) return false;
        if (!reg.beforeInsert(newConn)) return false;

        if (!reg.storeRemovePairImpl(leftId, oldRightId)) return false;
        if (!reg.storeAddImpl(newConn)) return false;

        reg.notifyItemsChanged();

        reg.onRemoveLog(oldCopy);
        reg.onInsertLog(newConn);

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
