#pragma once

//#include <functional>
#include <QString>
//#include "common/logger/logger.h"

template<typename Host, typename Connection>
struct ConnectionCrudMixin {
public:
    bool insertRaw(const Connection& c) {
        Host& reg = static_cast<Host&>(*this);
        Connection copy = c;

        if (!reg.storeAddImpl(copy))
            return false;

        reg.notifyItemsChanged();
        return true;
    }

    bool removeRaw(const typename Connection::IdType& leftId,
                   const typename Connection::IdType& rightId)
    {
        Host& reg = static_cast<Host&>(*this);

        if (!reg.storeRemovePairImpl(leftId, rightId))
            return false;

        reg.notifyItemsChanged();
        return true;
    }
};

// template<typename Host, typename Connection>
// struct ConnectionCrudMixin {
//     using IdType = typename Connection::IdType;

// private:
//     template<typename F>
//     bool runOperation(const char* opName, F&& fn) {
//         Host& reg = static_cast<Host&>(*this);
//         const QString name = [&]{
//             if constexpr (std::is_invocable_r_v<QString, decltype(&Host::registryName), Host>)
//                 return reg.registryName();
//             else
//                 return QStringLiteral("Registry");
//         }();
//         zInfo(QString("▶️ %1 operation start on %2").arg(opName, name));
//         try {
//             bool res = fn();
//             zInfo(QString("✅ %1 operation finished on %2: %3")
//                       .arg(opName, name, res ? "OK" : "REJECTED"));
//             return res;
//         } catch (const std::exception& ex) {
//             zError(QString("%1 operation exception on %2: %3")
//                        .arg(opName, name, QString::fromStdString(ex.what())));
//             return false;
//         } catch (...) {
//             zError(QString("%1 operation unknown exception on %2").arg(opName, name));
//             return false;
//         }
//     }

// public:
//     // INSERT kapcsolat – tiszta CRUD + log
//     bool insert(const Connection& c) {
//         return runOperation("connect-insert", [&]()->bool {
//             Host& reg = static_cast<Host&>(*this);
//             Connection copy = c;
//             return reg.add(copy);               // RegistryEngineBase::add() már onItemsChanged-et hív
//         });
//     }

//     // REMOVE kapcsolat (kompozit kulcs) – tiszta CRUD + log
//     bool remove(const IdType& leftId, const IdType& rightId) {
//         return runOperation("connect-remove", [&]()->bool {
//             Host& reg = static_cast<Host&>(*this);
//             using C = Connection;

//             const C* found = reg.findByPair(leftId, rightId);
//             if (!found)
//                 return false;

//             return reg.removeInternal(leftId, rightId);  // RegistryEngineBase::removeInternal() már onItemsChanged-et hív
//         });
//     }

//     // UPDATE nem támogatott kapcsolatoknál
//     bool update(const Connection&) {
//         zWarning("ConnectionCrudMixin: update() not supported for connection registries");
//         return false;
//     }
// };
