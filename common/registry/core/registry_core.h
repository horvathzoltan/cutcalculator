// // common/registry/core/registry_core.h
// #pragma once

// #include <QVector>

// template<typename Host, typename Entity>
// struct RegistryCore {
// protected:
//     using IdType = typename Entity::IdType;

//     bool addImpl(const Entity& e) {
//         return static_cast<Host*>(this)->storeAddImpl(e);
//     }

//     bool addAllImpl(const QVector<Entity>& v) {
//         return static_cast<Host*>(this)->storeAddAllImpl(v);
//     }

//     QVector<Entity> readAllImpl() const {
//         return static_cast<const Host*>(this)->storeReadAllImpl();
//     }

//     const Entity* findByIdImpl(const IdType& id) const {
//         return static_cast<const Host*>(this)->storeFindByIdImpl(id);
//     }

//     bool updateImpl(const Entity& e) {
//         return static_cast<Host*>(this)->storeUpdateImpl(e);
//     }

//     bool removeImpl(const IdType& id) {
//         return static_cast<Host*>(this)->storeRemoveImpl(id);
//     }
// };

