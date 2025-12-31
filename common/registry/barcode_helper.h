#pragma once
#include <QVector>
#include <QUuid>
#include "common/model/barcode_identifiable_entity.h"

struct BarcodeHelper {
    template<typename T>
    static const BarcodeIdentifiableEntity* findEntityById(const QVector<T>& items,
                                                           const QUuid& id)
    {
        for (const auto& item : items)
            if (item.id == id)
                return &item;
        return nullptr;
    }

    template<typename T>
    static T* findByBarcode(QVector<T>& items, const QString& barcode) {
        for (auto& item : items)
            if (item.barcode == barcode)
                return &item;
        return nullptr;
    }
};
