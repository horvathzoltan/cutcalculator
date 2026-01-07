#pragma once
#include "common/registry/feature/registry_base.h"
#include <QVector>
#include <QUuid>

template<typename TEntity>
class RegistryEngineBase : public RegistryBase {
public:
    using Base = RegistryBase;
    using EntityType = TEntity;

    RegistryEngineBase(const QString& registryName,
                       const QString& entityTypeName)
        : RegistryBase(registryName, entityTypeName)
    {}

// --- Internal CRUD a workflow policy számára ---
    bool insertInternal(const TEntity& e) {
        guardInstanceUsage();
        _items.append(e);
        return true;
    }

    bool updateInternal(const TEntity& e) {
        guardInstanceUsage();
        for (auto& item : _items) {
            if (item.id == e.id) {   // IdentifiableEntity szerződés
                item = e;
                return true;
            }
        }
        return false; // nem találtuk
    }

    bool removeInternal(const QUuid& id) {
        guardInstanceUsage();
        for (int i = 0; i < _items.size(); ++i) {
            if (_items[i].id == id) {
                _items.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool removeInternal(const QUuid& leftId, const QUuid& rightId) {
        guardInstanceUsage();
        for (int i = 0; i < _items.size(); ++i) {
            if (_items[i].leftId == leftId && _items[i].rightId == rightId) {
                _items.removeAt(i);
                return true;
            }
        }
        return false;
    }
    // --- CRUD storage primitives ---

    bool add(const TEntity& e) {
        guardInstanceUsage();
        _items.append(e);
        return true;
    }

    bool addAll(const QVector<TEntity>& v) {
        guardInstanceUsage();
        _items.append(v);
        return true;
    }  

    void setAll(const QVector<TEntity>& v) {
        guardInstanceUsage();
        _items = v;

        onAfterSetAll();
    }


    // --- READ APIs ---

    // POINTERES ALL() → workflow + domain hookoknak
    QVector<const TEntity*> all() const {
        guardInstanceUsage();
        QVector<const TEntity*> out;
        out.reserve(_items.size());
        for (const auto& item : _items)
            out.append(&item);
        return out;
    }

    // MÁSOLATOS READALL() → repositoryknak
    QVector<TEntity> readAll() const {
        guardInstanceUsage();
        return _items;
    }

    int size() const override {
        guardInstanceUsage();
        return _items.size();
    }

    bool isEmpty() const {
        guardInstanceUsage();
        return _items.isEmpty();
    }

    // --- Iterátor API ---
    auto begin() const { guardInstanceUsage(); return _items.begin(); }
    auto end()   const { guardInstanceUsage(); return _items.end(); }

    auto cbegin() const { guardInstanceUsage(); return _items.cbegin(); }
    auto cend()   const { guardInstanceUsage(); return _items.cend(); }

    // --- FIND APIs ---

    // find by predicate → pointer
    template<typename Predicate>
    const TEntity* findIf(Predicate&& pred) const {
        guardInstanceUsage();
        for (const auto& item : _items)
            if (pred(item))
                return &item;
        return nullptr;
    }

    // find all by predicate → QVector
    template<typename Predicate>
    QVector<TEntity> findAll(Predicate&& pred) const {
        guardInstanceUsage();
        QVector<TEntity> out;
        out.reserve(_items.size());
        for (const auto& item : _items)
            if (pred(item))
                out.append(item);
        return out;
    }

    // exists by predicate
    template<typename Predicate>
    bool existsBy(Predicate&& pred) const {
        return findIf(std::forward<Predicate>(pred)) != nullptr;
    }

    // --- FIND BY ID (kötelező minden IdentifiableEntity-hez) ---
    const TEntity* findById(const QUuid& id) const {
        guardInstanceUsage();
        for (const auto& item : _items)
            if (item.id == id)
                return &item;
        return nullptr;
    }

    bool existsById(const QUuid& id) const {
        return findById(id) != nullptr;
    }

    // --- ConnectionEntity-specifikus helper: findByPair ---
    const TEntity* findByPair(const QUuid& leftId, const QUuid& rightId) const {
        guardInstanceUsage();
        for (const auto& item : _items) {
            if (item.leftId == leftId && item.rightId == rightId)
                return &item;
        }
        return nullptr;
    }

// Hooks
protected:
    virtual void onAfterSetAll() {}

protected:
    QVector<TEntity> _items;
};
