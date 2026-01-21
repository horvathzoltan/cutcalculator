#pragma once
#include "common/registry/feature/registry_base.h"
#include <QVector>
#include <QUuid>
#include <functional>
#include <atomic>
#include <QReadWriteLock>
#include <memory>
#include "common/registry/subscription_token.h"
#include "common/logger/logger.h"

template<typename TEntity>
class RegistryEngineBase : public RegistryBase {
public:
    template<typename, typename>
    friend struct RegistryCore;

    using ItemsChangedEvent = std::function<void()>;
    using SubscriptionId = size_t;
    using IdType = typename TEntity::IdType;
    using EntityType = TEntity;   // vagy MaterialMaster, ProductMaster stb.

    RegistryEngineBase(const QString& registryName,
                       const QString& entityTypeName)
        : RegistryBase(registryName, entityTypeName)
        , _items(this)
    {}

    void notifyItemsChanged() {
        onItemsChanged();
    }

    // --- Event subscription API (thread-safe) ---

    SubscriptionId subscribeItemsChanged(ItemsChangedEvent cb) {
        const SubscriptionId id = _nextSubscriptionId.fetch_add(1, std::memory_order_relaxed);
        {
            QWriteLocker w(&_rwLock);
            _itemsChangedSubscribers.push_back({id, std::move(cb)});
        }
        return id;
    }

    void unsubscribeItemsChanged(SubscriptionId id) {
        QWriteLocker w(&_rwLock);
        for (int i = 0; i < _itemsChangedSubscribers.size(); ++i) {
            if (_itemsChangedSubscribers[i].first == id) {
                _itemsChangedSubscribers.removeAt(i);
                return;
            }
        }
    }

    SubscriptionToken subscribeItemsChangedToken(ItemsChangedEvent cb) {
        const SubscriptionId id = subscribeItemsChanged(std::move(cb));
        RegistryEngineBase* owner = this;
        return SubscriptionToken([owner, id]() {
            if (owner) owner->unsubscribeItemsChanged(id);
        });
    }

    SubscriptionToken subscribeItemsChangedTokenShared(std::shared_ptr<RegistryEngineBase> ownerShared,
                                                       ItemsChangedEvent cb) {
        std::weak_ptr<RegistryEngineBase> weakOwner = ownerShared;
        const SubscriptionId id = subscribeItemsChanged(std::move(cb));
        return SubscriptionToken([weakOwner, id]() {
            if (auto owner = weakOwner.lock()) {
                owner->unsubscribeItemsChanged(id);
            }
        });
    }

public:
    const QString& registryName() const { return _registryName; }

    // --- READ APIs (delegáló thin wrappers) ---
    QVector<TEntity> readAll() const {
        guardInstanceUsage();
        return storeReadAllImpl();
    }

    QVector<const TEntity*> all() const {
        guardInstanceUsage();
        QReadLocker r(&_rwLock);
        QVector<const TEntity*> out;
        const auto& raw = _items.data();
        out.reserve(raw.size());
        for (const auto& item : raw)
            out.append(&item);
        return out;
    }

    int size() const override {
        guardInstanceUsage();
        QReadLocker r(&_rwLock);
        return _items.data().size();
    }

    bool isEmpty() const {
        guardInstanceUsage();
        QReadLocker r(&_rwLock);
        return _items.data().isEmpty();
    }

    template<typename Predicate>
    const TEntity* findIf(Predicate&& pred) const {
        guardInstanceUsage();
        QReadLocker r(&_rwLock);
        for (const auto& item : _items.data())
            if (pred(item))
                return &item;
        return nullptr;
    }

    template<typename Predicate>
    QVector<TEntity> findAll(Predicate&& pred) const {
        guardInstanceUsage();
        QReadLocker r(&_rwLock);
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

    const TEntity* findById(const IdType& id) const {
        guardInstanceUsage();
        return storeFindByIdImpl(id);
    }

    bool existsById(const IdType& id) const {
        return findById(id) != nullptr;
    }

    const TEntity* findByPair(const IdType& leftId, const IdType& rightId) const {
        guardInstanceUsage();
        return storeFindByPairImpl(leftId, rightId);
    }

    // --- WRITE APIs (thin wrappers that notify) ---
    bool add(const TEntity& e) {
        guardInstanceUsage();
        if (!storeAddImpl(e)) return false;
        onItemsChanged();
        return true;
    }

    bool addAll(const QVector<TEntity>& v) {
        guardInstanceUsage();
        if (!storeAddAllImpl(v)) return false;
        onItemsChanged();
        return true;
    }

    void setAll(const QVector<TEntity>& v) {
        guardInstanceUsage();
        storeSetAllImpl(v);
        onItemsChanged();
    }

    // Internal names kept for compatibility with existing code
    // bool insertInternal(const TEntity& e) { return add(e); }

    bool updateInternal(const TEntity& e) {
        guardInstanceUsage();
        if (!storeUpdateImpl(e)) return false;
        onItemsChanged();
        return true;
    }

    bool removeInternal(const IdType& id) {
        guardInstanceUsage();
        if (!storeRemoveImpl(id)) return false;
        onItemsChanged();
        return true;
    }

    bool removeInternal(const IdType& leftId, const IdType& rightId) {
        guardInstanceUsage();
        if (!storeRemovePairImpl(leftId, rightId)) return false;
        onItemsChanged();
        return true;
    }

protected:
    // --- Central event trigger (thread-safe, reentrancy-safe) ---
    virtual void onItemsChanged(){
        zInfo("róka1: RegistryEngineBase::onItemsChanged belépés");

        QVector<std::pair<SubscriptionId, ItemsChangedEvent>> subsCopy;
        {
            QReadLocker r(&_rwLock);
            subsCopy = _itemsChangedSubscribers;
        }
        for (const auto& p : subsCopy) {
            try {
                if (p.second) p.second();
            } catch (...) {
                // swallow exceptions from callbacks to keep engine stable
            }
        }

        zInfo("róka2: RegistryEngineBase::onItemsChanged kilépés");
    }

    // --- Protected store*Impl methods used by RegistryCore and mixins ---
    bool storeAddImpl(const TEntity& e) {
        QWriteLocker w(&_rwLock);
        _items.append(e);
        return true;
    }

    bool storeAddAllImpl(const QVector<TEntity>& v) {
        QWriteLocker w(&_rwLock);
        _items.append(v);
        return true;
    }

    void storeSetAllImpl(const QVector<TEntity>& v) {
        QWriteLocker w(&_rwLock);
        _items.setAll(v);
    }

    QVector<TEntity> storeReadAllImpl() const {
        QReadLocker r(&_rwLock);
        return _items.data();
    }

    const TEntity* storeFindByIdImpl(const IdType& id) const {
        QReadLocker r(&_rwLock);
        for (const auto& item : _items.data())
            if (item.id == id) return &item;
        return nullptr;
    }

    const TEntity* storeFindByPairImpl(const IdType& leftId, const IdType& rightId) const {
        QReadLocker r(&_rwLock);
        for (const auto& item : _items.data())
            if (item.leftId == leftId && item.rightId == rightId) return &item;
        return nullptr;
    }

    bool storeUpdateImpl(const TEntity& e) {
        QWriteLocker w(&_rwLock);
        auto& raw = _items.data();
        for (auto& item : raw) {
            if (item.id == e.id) { item = e; return true; }
        }
        return false;
    }

    bool storeRemoveImpl(const IdType& id) {
        QWriteLocker w(&_rwLock);
        auto& raw = _items.data();
        for (int i = 0; i < raw.size(); ++i) {
            if (raw[i].id == id) { _items.removeAt(i); return true; }
        }
        return false;
    }

    bool storeRemovePairImpl(const IdType& leftId, const IdType& rightId) {
        QWriteLocker w(&_rwLock);
        auto& raw = _items.data();
        for (int i = 0; i < raw.size(); ++i) {
            if (raw[i].leftId == leftId && raw[i].rightId == rightId) { _items.removeAt(i); return true; }
        }
        return false;
    }

    // --- Internal storage wrapper ---
    class ItemStore {
    public:
        explicit ItemStore(RegistryEngineBase* owner)
            : _owner(owner)
        {}

        void setAll(const QVector<TEntity>& v) {
            _items = v;
        }

        void append(const TEntity& e) {
            _items.append(e);
        }

        void append(const QVector<TEntity>& v) {
            _items.append(v);
        }

        void removeAt(int i) {
            _items.removeAt(i);
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
    mutable QReadWriteLock _rwLock;
    QVector<std::pair<SubscriptionId, ItemsChangedEvent>> _itemsChangedSubscribers;
    std::atomic<SubscriptionId> _nextSubscriptionId{1};
};
