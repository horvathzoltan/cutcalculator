#pragma once
#include <QUuid>
#include <QString>
#include "common/model/barcode_identifiable_entity.h"

class IBarcodeIdentifiableRegistryBase {
public:
    virtual ~IBarcodeIdentifiableRegistryBase() = default;
    virtual QString typeName() const = 0;
    virtual const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const = 0;
};

// class IHierarchicalRegistryBase {
// public:
//     virtual ~IHierarchicalRegistryBase() = default;
//     //virtual QString typeName() const = 0;
//     //virtual const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const = 0;
//     virtual QVector<T> findChildren(const QVector<T>& items, const QUuid& parentId) = 0;
//     virtual QVector<T> roots(const QVector<T>& items) = 0;
// };
