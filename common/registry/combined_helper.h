#pragma once
#include "hierarchy_helper.h"
#include "barcode_helper.h"

struct CombinedHelper {
    template<typename T>
    static QVector<T> findChildren(const QVector<T>& items, const QUuid& parentId) {
        return HierarchyHelper::findChildren(items, parentId);
    }

    template<typename T>
    static QVector<T> roots(const QVector<T>& items) {
        return HierarchyHelper::roots(items);
    }

    template<typename T>
    static const BarcodeIdentifiableEntity* findEntityById(const QVector<T>& items,
                                                           const QUuid& id)
    {
        return BarcodeHelper::findEntityById(items, id);
    }
};
