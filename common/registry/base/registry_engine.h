#pragma once

#include "registry_engine_base.h"

template<typename TEntity, typename WorkflowPolicy>
class RegistryEngine
    : public RegistryEngineBase<TEntity>
    , public WorkflowPolicy
{
public:
    using Base = RegistryEngineBase<TEntity>;
    using EntityType = TEntity;
    using Workflow = WorkflowPolicy;

    RegistryEngine(const QString& name, const QString& typeName)
        : Base(name, typeName)
    {}

    bool insert(const TEntity& e) {
        return Workflow::insert(*this, e);
    }

    bool update(const TEntity& e) {
        return Workflow::update(*this, e);
    }

    bool remove(const QUuid& id) {
        return Workflow::remove(*this, id);
    }

    bool load(const QVector<TEntity>& items) {
        return Workflow::load(*this, items);
    }

    bool remove(const QUuid& leftId, const QUuid& rightId) {
        return WorkflowPolicy::remove(*this, leftId, rightId);
    }


    // --- DOMAIN HOOKOK (alapértelmezésben no-op) ---
    virtual bool validateDomain(const TEntity&) const { return true; }
    virtual bool validateDuplicate(const TEntity&) const { return true; }

    virtual bool beforeInsert(const TEntity&) { return true; }
    virtual void afterInsert(const TEntity&) {}

    virtual bool beforeUpdate(const TEntity&) { return true; }
    virtual void afterUpdate(const TEntity&) {}

    virtual bool beforeRemove(const TEntity&) { return true; }
    virtual void afterRemove(const TEntity&) {}

    virtual void onInsertLog(const TEntity&) {}
    virtual void onUpdateLog(const TEntity&) {}
    virtual void onRemoveLog(const TEntity&) {}

    // 🔄 Load workflow log hook – alapértelmezésben no-op
    virtual void onLoadLog() {}

    // --- Persist hook (workflow számára) ---
    // Alapértelmezésben no-op; a konkrét registryk override-olhatják.
    virtual void persist() const {}

    // virtual bool beforeRemovePair(const QUuid&, const QUuid&) { return true; }
    // virtual void afterRemovePair(const QUuid&, const QUuid&) {}
    // virtual void onRemovePairLog(const QUuid&, const QUuid&) {}

    // 🔗 Connection-specifikus hook – alapértelmezésben no-op
    virtual bool validateConnection(const TEntity&) const { return true; }
};

// #pragma once

// #include "common/registry/base/registry_engine_base.h"

// // 🧩 RegistryEngine<TEntity, WorkflowPolicy>
// // - összeköti a domain-engine-t és a workflow policy-t
// // - a workflow policy végzi a műveleteket
// // - a registry engine csak domain hookokat és tárolást ad

// template<typename Derived, typename TEntity, typename WorkflowPolicy>
// class RegistryEngine : public RegistryEngineBase<Derived, TEntity>,
//                        public WorkflowPolicy
// {
// public:
//     using Base = RegistryEngineBase<Derived, TEntity>;
//     using EntityType = TEntity;
//     using Workflow = WorkflowPolicy;

//     RegistryEngine(const QString& name, const QString& typeName)
//         : Base(name, typeName)
//     {}

//     // --- CRUD workflow delegálás ---
//     bool insert(const TEntity& e) {
//         return Workflow::insert(*this, e);
//     }

//     bool update(const TEntity& e) {
//         return Workflow::update(*this, e);
//     }

//     bool remove(const QUuid& id) {
//         return Workflow::remove(*this, id);
//     }

//     // --- Connection workflow delegálás ---
//     template<typename Left, typename Right>
//     bool remove(const Left& leftId, const Right& rightId) {
//         return Workflow::remove(*this, leftId, rightId);
//     }

//     // --- Barcode workflow delegálás ---
//     bool registerNew(const TEntity& r) {
//         return Workflow::registerNew(*this, r);
//     }

//     bool retire(const QString& code, const QString& reason) {
//         return Workflow::retire(*this, code, reason);
//     }

//     // --- Lookup workflow delegálás ---
//     bool load(const QVector<TEntity>& items) {
//         return Workflow::load(*this, items);
//     }

//     // --- Domain hookok (alapértelmezésben no-op) ---
//     virtual bool validateDomain(const TEntity&) const { return true; }
//     virtual bool validateDuplicate(const TEntity&) const { return true; }

//     virtual bool validateConnection(const TEntity&) const { return true; }

//     virtual bool validateBarcode(const TEntity&) const { return true; }
//     virtual bool validateCollision(const TEntity&) const { return true; }

//     virtual bool beforeInsert(const TEntity&) { return true; }
//     virtual void afterInsert(const TEntity&) {}

//     virtual bool beforeUpdate(const TEntity&) { return true; }
//     virtual void afterUpdate(const TEntity&) {}

//     virtual bool beforeRemove(const TEntity&) { return true; }
//     virtual void afterRemove(const TEntity&) {}

//     virtual void onInsertLog(const TEntity&) {}
//     virtual void onUpdateLog(const TEntity&) {}
//     virtual void onRemoveLog(const TEntity&) {}

//     virtual void onRegisterLog(const TEntity&) {}
//     virtual void onRetireLog(const TEntity&, const QString&) {}

//     virtual void onLoadLog() {}

//     // --- Persist hook ---
//     virtual void persist() const {}
// };
