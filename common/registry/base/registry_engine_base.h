#pragma once
#include "registry_base.h"
#include <QList>
#include "common/system/registry_catalog.h"

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
//private:
    //inline static bool _autoRegister = (RegistryCatalog::add<TDerived>(), true);
protected:
    QList<TEntity> _items;
};
