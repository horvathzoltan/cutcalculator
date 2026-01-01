#pragma once
#include "registry_engine_base.h"
#include <QUuid>

template<typename T>
class IdentifiableRegistryEngine : public RegistryEngineBase<T>
{
public:
    using Base = RegistryEngineBase<T>;
    using Base::Base;

    // QString typeName() const override {
    //     return Base::typeName();
    // }

    //CRUD
    bool update(const T& e) {
        Base::guardInstanceUsage();
        for (int i = 0; i < Base::_items.size(); ++i) {
            if (this->_items[i].id == e.id) {
                this->_items[i] = e;
                return true;
            }
        }
        return false;
    }

    bool remove(const QUuid& id) {
        Base::guardInstanceUsage();
        for (int i = 0; i < Base::_items.size(); ++i) {
            if (this->_items[i].id == id) {
                this->_items.removeAt(i);
                return true;
            }
        }
        return false;
    }

    //find

    const T* findById(const QUuid& id) const {
        Base::guardInstanceUsage();
        for (const auto& item : Base::_items) {
            if (item.id == id)
                return &item;
        }
        return nullptr;
    }
};
