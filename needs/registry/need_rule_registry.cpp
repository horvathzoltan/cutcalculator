#include "needs/registry/need_rule_registry.h"
#include "common/utils/scoped_per_thread_lock.h"
#include "common/utils/filename_helper.h"
#include "needs/repository/need_rule_repository.h"
#include "common/logger/event_logger.h"

NeedRuleRegistry& NeedRuleRegistry::instance() {
    static NeedRuleRegistry inst;
    return inst;
}

void NeedRuleRegistry::setData(const QVector<NeedRule>& v) {
    ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
    _data = v;
}

const QVector<NeedRule>& NeedRuleRegistry::readAll() const {
    return _data;
}

QVector<NeedRule> NeedRuleRegistry::findByProduct(const QUuid& productId) const {
    QVector<NeedRule> out;
    ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
    for (const auto& r : _data) {
        if (r.leftId == productId) out.append(r);
    }
    return out;
}

QVector<NeedRule> NeedRuleRegistry::findByMaterial(const QUuid& materialId) const {
    QVector<NeedRule> out;
    ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
    for (const auto& r : _data) {
        if (r.rightId == materialId) out.append(r);
    }
    return out;
}

bool NeedRuleRegistry::exists(const QUuid& productId, const QUuid& materialId) const {
    ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
    for (const auto& r : _data) {
        if (r.leftId == productId && r.rightId == materialId) return true;
    }
    return false;
}

void NeedRuleRegistry::insert(const NeedRule& rule) {
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
        // Dupla beszúrás ellenőrzése (természetes kulcs)
        for (const auto& r : _data) {
            if (r.leftId == rule.leftId && r.rightId == rule.rightId) {
                zEventWARN("⚠️ NeedRule already exists (duplicate connection prevented)");
                return;
            }
        }
        _data.append(rule);
    }
    persist();
    zEventINFO("➕ NeedRule inserted (Product↔Material)");
}

bool NeedRuleRegistry::remove(const QUuid& productId, const QUuid& materialId) {
    bool removed = false;
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
        for (int i = 0; i < _data.size(); ++i) {
            if (_data[i].leftId == productId && _data[i].rightId == materialId) {
                _data.remove(i);
                removed = true;
                break;
            }
        }
    }
    if (removed) {
        persist();
        zEventINFO("🗑️ NeedRule removed (Product↔Material)");
    }
    return removed;
}

void NeedRuleRegistry::persist() const {
    const QString path = FileNameHelper::instance().getNeedRuleCsvFile(); // pl. needs_rules.csv
    if (path.isEmpty()) {
        zEventWARN("❌ NeedRule persist skipped: empty path");
        return;
    }

    QVector<NeedRule> snapshot;
    {
        ScopedPerThreadLock locker(static_cast<void*>(&_mutex), /*recursive=*/true);
        snapshot = _data;
    }

    NeedRuleRepository::saveToCSV(snapshot, path);
    zEventINFO(QString("💾 NeedRule persisted → %1 rules").arg(snapshot.size()));
}
