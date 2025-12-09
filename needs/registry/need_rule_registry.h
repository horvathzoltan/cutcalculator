#pragma once
#include <QVector>
#include <QRecursiveMutex>
#include "common/registry/registry_base.h"
#include "needs/model/need_rule.h"

/**
 * NeedRuleRegistry
 *
 * Hunglish:
 * - In-memory tároló a NeedRule kapcsolatokhoz (Product ↔ Material).
 * - Thread-safe (QRecursiveMutex), auditbarát CRUD + lekérdezések.
 * - Persist() explicit hívással CSV-be ment (repo-n keresztül).
 *
 * Elhelyezés:
 * - needs/registry/need_rule_registry.h/.cpp
 */
class NeedRuleRegistry : public RegistryBase {
private:
    NeedRuleRegistry() : RegistryBase("NeedRuleRegistry") {}
    NeedRuleRegistry(const NeedRuleRegistry&) = delete;

    QVector<NeedRule> _data;
    mutable QRecursiveMutex _mutex;

public:
    static NeedRuleRegistry& instance();

    QString typeName() const override { return "NeedRule"; }
    int size() const override { return _data.size(); }

    // Teljes tömb beállítása (import után)
    void setData(const QVector<NeedRule>& v);
    // Csak olvasásra: teljes tömb
    const QVector<NeedRule>& readAll() const;

    // Lekérdezések
    QVector<NeedRule> findByProduct(const QUuid& productId) const;
    QVector<NeedRule> findByMaterial(const QUuid& materialId) const;

    // CRUD
    void insert(const NeedRule& rule);
    bool remove(const QUuid& productId, const QUuid& materialId);
    bool exists(const QUuid& productId, const QUuid& materialId) const;

    // Perzisztálás CSV-be (Repository-n át)
    void persist() const;
};
