#pragma once
#include "common/registry/base/identifiable_registry_engine.h"

template<typename T>
class HierarchicalRegistryEngine : public IdentifiableRegistryEngine<T>
//,public IHierarchicalRegistryBase<T>
{
    using Base = IdentifiableRegistryEngine<T>;
    using Base::Base;
public:
    QVector<T> findChildren(const QUuid& parentId) {
        Base::guardInstanceUsage();
        QVector<T> result;
        for (const auto& item : Base::_items)
            if (item.parentId == parentId)
                result.append(item);
        return result;
    }

    QVector<T> roots() {
        Base::guardInstanceUsage();
        QVector<T> result;
        for (const auto& item : Base::_items)
            if (item.parentId.isNull())
                result.append(item);
        return result;
    }
};
