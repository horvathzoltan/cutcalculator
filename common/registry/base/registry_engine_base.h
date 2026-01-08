#pragma once
#include "common/registry/feature/registry_base.h"
#include <QVector>
#include <QUuid>
#include <functional>

template<typename TEntity>
class RegistryEngineBase : public RegistryBase {
public:
    using ItemsChangedEvent = std::function<void()>;

    RegistryEngineBase(const QString& registryName,
                       const QString& entityTypeName)
        : RegistryBase(registryName, entityTypeName)
        , _items(this)
    {}

    // --- Event subscription API ---
    void subscribeItemsChanged(ItemsChangedEvent cb) {
        _itemsChangedSubscribers.push_back(std::move(cb));
    }

public:
    // --- Internal CRUD ---
    bool insertInternal(const TEntity& e) {
        guardInstanceUsage();
        _items.append(e);
        return true;
    }

    bool updateInternal(const TEntity& e) {
        guardInstanceUsage();
        for (auto& item : _items.data()) {
            if (item.id == e.id) {
                item = e;
                onItemsChanged();
                return true;
            }
        }
        return false;
    }

    bool removeInternal(const QUuid& id) {
        guardInstanceUsage();
        auto& raw = _items.data();
        for (int i = 0; i < raw.size(); ++i) {
            if (raw[i].id == id) {
                _items.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool removeInternal(const QUuid& leftId, const QUuid& rightId) {
        guardInstanceUsage();
        auto& raw = _items.data();
        for (int i = 0; i < raw.size(); ++i) {
            if (raw[i].leftId == leftId && raw[i].rightId == rightId) {
                _items.removeAt(i);
                return true;
            }
        }
        return false;
    }

public:
    // --- READ APIs ---
    QVector<TEntity> readAll() const {
        guardInstanceUsage();
        return _items.data();
    }

    QVector<const TEntity*> all() const {
        guardInstanceUsage();
        QVector<const TEntity*> out;
        const auto& raw = _items.data();
        out.reserve(raw.size());
        for (const auto& item : raw)
            out.append(&item);
        return out;
    }

    int size() const override {
        guardInstanceUsage();
        return _items.data().size();
    }

// ezeket át kell majd gondolni - régi API visszahozva:
    bool isEmpty() const {
        guardInstanceUsage();
        return _items.data().isEmpty();
    }

    auto begin() const { guardInstanceUsage(); return _items.data().begin(); }
    auto end()   const { guardInstanceUsage(); return _items.data().end(); }

    auto cbegin() const { guardInstanceUsage(); return _items.data().cbegin(); }
    auto cend()   const { guardInstanceUsage(); return _items.data().cend(); }

    template<typename Predicate>
    const TEntity* findIf(Predicate&& pred) const {
        guardInstanceUsage();
        for (const auto& item : _items.data())
            if (pred(item))
                return &item;
        return nullptr;
    }

    template<typename Predicate>
    QVector<TEntity> findAll(Predicate&& pred) const {
        guardInstanceUsage();
        QVector<TEntity> out;
        const auto& raw = _items.data();
        out.reserve(raw.size());
        for (const auto& item : raw)
            if (pred(item))
                out.append(item);
        return out;
    }

    template<typename Predicate>
    bool existsBy(Predicate&& pred) const {
        return findIf(std::forward<Predicate>(pred)) != nullptr;
    }

    const TEntity* findById(const QUuid& id) const {
        guardInstanceUsage();
        for (const auto& item : _items.data())
            if (item.id == id)
                return &item;
        return nullptr;
    }

    bool existsById(const QUuid& id) const {
        return findById(id) != nullptr;
    }

    const TEntity* findByPair(const QUuid& leftId, const QUuid& rightId) const {
        guardInstanceUsage();
        for (const auto& item : _items.data()) {
            if (item.leftId == leftId && item.rightId == rightId)
                return &item;
        }
        return nullptr;
    }
public:
    // --- WRITE APIs (régi API visszahozva) ---

    bool add(const TEntity& e) {
        guardInstanceUsage();
        _items.append(e);        // ItemStore::append → eventet lő
        return true;
    }

    bool addAll(const QVector<TEntity>& v) {
        guardInstanceUsage();
        _items.append(v);        // ItemStore::append → eventet lő
        return true;
    }

    void setAll(const QVector<TEntity>& v) {
        guardInstanceUsage();
        _items.setAll(v);        // ItemStore::setAll → eventet lő
    }



protected:
    // --- Central event trigger ---
    virtual void onItemsChanged() {
        for (auto& cb : _itemsChangedSubscribers)
            cb();
    }

    // --- Internal storage wrapper ---
    class ItemStore {
    public:
        explicit ItemStore(RegistryEngineBase* owner)
            : _owner(owner)
        {}

        void setAll(const QVector<TEntity>& v) {
            _items = v;
            _owner->onItemsChanged();
        }

        void append(const TEntity& e) {
            _items.append(e);
            _owner->onItemsChanged();
        }

        void append(const QVector<TEntity>& v) {
            _items.append(v);
            _owner->onItemsChanged();
        }

        void removeAt(int i) {
            _items.removeAt(i);
            _owner->onItemsChanged();
        }

        QVector<TEntity>& data() { return _items; }
        const QVector<TEntity>& data() const { return _items; }

    private:
        RegistryEngineBase* _owner;
        QVector<TEntity> _items;
    };

protected:
    ItemStore _items;

private:
    QVector<ItemsChangedEvent> _itemsChangedSubscribers;
};
