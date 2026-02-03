// // common/registry/mixins/crud_mixin.h
// #pragma once

// // ⚠️ CrudMixin szerződés:
// // - runtime-only CRUD (add/update/remove) storage műveletek
// // - NEM persistál, NEM workflow
// // - domain registryk NE hívják közvetlenül
// // - a valódi CRUD út: CrudWorkflowMixin (validate + log + persist)

// #include "../core/registry_core.h"

// //#include "contract_checks.h"
// #include <QString>
// //#include <functional>
// #include <exception>
// #include "common/logger/logger.h"

// template<typename Host, typename Entity>
//     requires requires { typename Entity::IdType(); }
// struct CrudMixin : RegistryCore<Host, Entity> {
// private:
//     template<typename F>
//     bool runOperation(const QString& opName, F&& fn) {
//         Host& reg = static_cast<Host&>(*this);
//         zInfo(QString("%1 operation start on %2").arg(opName, reg.registryName()));
//         try {
//             bool res = fn();
//             zInfo(QString("%1 operation finished on %2: %3")
//                       .arg(opName, reg.registryName(), res ? "OK" : "REJECTED"));
//             return res;
//         } catch (const std::exception& ex) {
//             zError(QString("%1 operation exception on %2: %3")
//                        .arg(opName, reg.registryName(), QString::fromStdString(ex.what())));
//             return false;
//         } catch (...) {
//             zError(QString("%1 operation unknown exception on %2")
//                        .arg(opName, reg.registryName()));
//             return false;
//         }
//     }

// public:
//     [[nodiscard]]
//     bool insert(const Entity& e) {
//         return runOperation("insert", [&]()->bool {
//             Host& reg = static_cast<Host&>(*this);
//             Entity mutableCopy = e;
//             if (!this->addImpl(mutableCopy))
//                 return false;
//             reg.notifyItemsChanged();
//             return true;
//         });
//     }

//     [[nodiscard]]
//     bool update(const Entity& e) {
//         return runOperation("update", [&]()->bool {
//             Host& reg = static_cast<Host&>(*this);
//             Entity mutableCopy = e;
//             if (!this->updateImpl(mutableCopy))
//                 return false;
//             reg.notifyItemsChanged();
//             return true;
//         });
//     }

//     [[nodiscard]]
//     bool remove(const typename Entity::IdType& id) {
//         return runOperation("remove", [&]()->bool {
//             Host& reg = static_cast<Host&>(*this);
//             if (!this->removeImpl(id))
//                 return false;
//             reg.notifyItemsChanged();
//             return true;
//         });
//     }
// };
