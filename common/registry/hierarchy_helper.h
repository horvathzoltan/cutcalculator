#pragma once
#include <QVector>
#include <QUuid>

struct HierarchyHelper {
    template<typename T>
    static QVector<T> findChildren(const QVector<T>& items, const QUuid& parentId) {
        QVector<T> result;
        for (const auto& item : items)
            if (item.parentId == parentId)
                result.append(item);
        return result;
    }

    template<typename T>
    static QVector<T> roots(const QVector<T>& items) {
        QVector<T> result;
        for (const auto& item : items)
            if (item.parentId.isNull())
                result.append(item);
        return result;
    }
};
