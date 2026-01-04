#pragma once
#include "registry_engine_base.h"
#include <QUuid>
//#include "common/registry/base/identifiable_registry_helper.h"

template<typename T>
class IdentifiableRegistryEngine : public RegistryEngineBase<T>
{    
    friend struct IdentifiableRegistryHelper; // <-- EZ KELL

protected:
    //insert hookok
    virtual bool validateDomain(const T&) const { return true; }
    virtual bool validateDuplicate(const T&) const { return true; }
    virtual bool beforeInsert(const T&) { return true; }
    virtual void afterInsert(const T&) {}
    virtual void onInsertLog(const T&) {}
    // remove hookok
    virtual bool beforeRemove(const T&) { return true; }
    virtual void afterRemove(const T&) {}
    virtual void onRemoveLog(const T&) {}
    // update hookok
    virtual bool beforeUpdate(const T&) { return true; }
    virtual void afterUpdate(const T&) {}
    virtual void onUpdateLog(const T&) {}
    // persist hook – alapból no-op
    virtual void persist() const {}

    bool removeInternal(const QUuid& id) {
        for (int i = 0; i < this->_items.size(); ++i) {
            if (this->_items[i].id == id) {
                this->_items.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool updateInternal(const T& e) {
        for (auto& item : this->_items) {
            if (item.id == e.id) {
                item = e;
                return true;
            }
        }
        return false;
    }

public:
//    using entity_type = T;

    using Base = RegistryEngineBase<T>;
    using Base::Base;

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
