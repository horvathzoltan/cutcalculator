#pragma once

#include "common/registry/base/registry_engine_base.h"
#include <QUuid>
#include <QVector>

/**
 * 🧩 ConnectionRegistryEngine<ConnectionType>
 *
 * Generikus engine kapcsolótáblákhoz.
 * Elvárás: ConnectionType-nek legyen public QUuid leftId, rightId mezője
 * (pl. ConnectionEntity<LeftEntity, RightEntity> leszármazott).
 */
template<typename ConnectionType>
class ConnectionRegistryEngine : public RegistryEngineBase<ConnectionType>
{
public:
    using Base = RegistryEngineBase<ConnectionType>;
    using Base::Base; // ctor öröklés

    // 🔧 Saját setData – közvetlenül az engine _items mezőjére építve
    void setData(const QVector<ConnectionType>& v) {
        Base::guardInstanceUsage();
        Base::_items = v;
    }

    // 🔧 Saját readAll – auditbarát, guardolt olvasás
    const QVector<ConnectionType>& readAll() const {
        Base::guardInstanceUsage();
        return Base::_items;
    }

    // 👇 Beszúrás – engine add() wrapper
    void insert(const ConnectionType& c) {
        Base::add(c);
    }

    // 👇 Törlés left/right GUID pár alapján
    bool remove(const QUuid& left, const QUuid& right) {
        Base::guardInstanceUsage();

        bool removed = false;
        QVector<ConnectionType> newItems;
        newItems.reserve(Base::_items.size());

        for (const auto& item : Base::_items) {
            if (item.leftId == left && item.rightId == right) {
                removed = true;
                continue;
            }
            newItems.append(item);
        }

        if (removed)
            Base::_items = newItems;

        return removed;
    }

    // 👇 Szűrés bal oldal szerint
    QVector<ConnectionType> findByLeft(const QUuid& left) const {
        QVector<ConnectionType> result;
        Base::forEach([&](const ConnectionType& c){
            if (c.leftId == left)
                result.append(c);
        });
        return result;
    }

    // 👇 Szűrés jobb oldal szerint
    QVector<ConnectionType> findByRight(const QUuid& right) const {
        QVector<ConnectionType> result;
        Base::forEach([&](const ConnectionType& c){
            if (c.rightId == right)
                result.append(c);
        });
        return result;
    }
};
