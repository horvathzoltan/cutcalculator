#pragma once
#include "common/registry/barcode/ibarcode_identifiable_registry_base.h"
#include "common/registry/base/identifiable_registry_engine.h"
#include <QUuid>

template<typename T>
class BarcodeIdentifiableRegistryEngine : public IdentifiableRegistryEngine<T>,
                                   public IBarcodeIdentifiableRegistryBase
{
public:
    using Base = IdentifiableRegistryEngine<T>;
    using Base::Base;

    QString typeName() const override {
         return Base::typeName();
    }

    const BarcodeIdentifiableEntity* findEntityById(const QUuid& id) const override {
        //return BarcodeHelper::findEntityById(Base::_items, id);
        Base::guardInstanceUsage();
        for (const auto& item : Base::_items) {
            if (item.id == id)
                return reinterpret_cast<const BarcodeIdentifiableEntity*>(&item);
        }
        return nullptr;
    }

    const T* findByBarcode(const QString& barcode) const {
        //return BarcodeHelper::findByBarcode(Base::_items, barcode);
        Base::guardInstanceUsage();
        for (const auto& item : Base::_items) {
            if (item.barcode == barcode)
                return &item;
        }
        return nullptr;
    }
};
