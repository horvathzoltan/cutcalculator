#pragma once
#include "common/registry/feature/registry_base.h"
#include <QList>

template<typename TEntity>//, typename TDerived>
class RegistryEngineBase : public RegistryBase {
public:
    using Base = RegistryBase;
    using EntityType = TEntity;

    RegistryEngineBase(const QString& registryName,
                       const QString& entityTypeName)
        : RegistryBase(registryName, entityTypeName)
    {}

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
        _items.clear();
        _items.append(v);
    }

    QList<const TEntity*> all() const {
        guardInstanceUsage();
        QList<const TEntity*> out;
        out.reserve(_items.size());
        for (const auto& item : _items)
            out.append(&item);
        return out;
    }

    QList<TEntity> readAll() const {
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

    template<typename Func>
    void forEach(Func&& f) const {
        guardInstanceUsage();
        for (const auto& item : _items)
            f(item);
    }

    template<typename Predicate>
    const TEntity* findIf(Predicate&& pred) const {
        guardInstanceUsage();
        for (const auto& item : _items) {
            if (pred(item))
                return &item;
        }
        return nullptr;
    }

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

    //
    template<typename Predicate>
    bool existsBy(Predicate&& pred) const {
        return findIf(std::forward<Predicate>(pred)) != nullptr;
    }


//private:
    //inline static bool _autoRegister = (RegistryCatalog::add<TDerived>(), true);
protected:
    QList<TEntity> _items;
};
