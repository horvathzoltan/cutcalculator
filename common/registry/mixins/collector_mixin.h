// common/registry/mixins/collector_mixin.h
#pragma once

#include "../core/registry_core.h"
#include "contract_checks.h"
#include <QString>
#include "common/logger/logger.h"

template<typename Host, typename Entity>
struct CollectorMixin : RegistryCore<Host, Entity> {
    CollectorContractChecks<Host, Entity> _contract_check;

private:
    template<typename F>
    bool runOperation(const char* opName, F&& fn) {
        Host& reg = static_cast<Host&>(*this);
        const QString name = reg.registryName();
        zInfo(QString("▶️ %1 operation start on %2").arg(opName, name));
        try {
            bool res = fn();
            zInfo(QString("✅ %1 operation finished on %2: %3")
                      .arg(opName, name, res ? "OK" : "REJECTED"));
            return res;
        } catch (const std::exception& ex) {
            zError(QString("%1 operation exception on %2: %3")
                       .arg(opName, name, QString::fromStdString(ex.what())));
            return false;
        } catch (...) {
            zError(QString("%1 operation unknown exception on %2").arg(opName, name));
            return false;
        }
    }

    bool add(const Entity& e) {
        return runOperation("collector-add", [&]()->bool {
            Host& reg = static_cast<Host&>(*this);
            Entity mutableCopy = e;

            if (!reg.beforeInsertValidate(mutableCopy)) return false;
            if (!this->addImpl(mutableCopy))    return false;

            reg.afterInsert(mutableCopy);
            reg.onInsertLog(mutableCopy);
            reg.persistRegistry();
            reg.notifyItemsChanged();
            return true;
        });
    }

    bool addAll(const QVector<Entity>& v) {
        Host& reg = static_cast<Host&>(*this);
        if (!this->addAllImpl(v)) return false;
        reg.persistRegistry();
        reg.notifyItemsChanged();
        return true;
    }

    // Optional: host can override importCsvImpl for custom parsing
    bool importCsv(const QString& path) {
        Host& reg = static_cast<Host&>(*this);
        return reg.importCsvImpl(path);
    }

    bool update(const Entity& e) {
        return runOperation("collector-update", [&]()->bool {
            Host& reg = static_cast<Host&>(*this);
            Entity mutableCopy = e;

            if (!reg.beforeUpdateValidate(mutableCopy)) return false;
            if (!this->updateImpl(mutableCopy))  return false;

            reg.afterUpdate(mutableCopy);
            reg.onUpdateLog(mutableCopy);
            reg.persistRegistry();
            reg.notifyItemsChanged();
            return true;
        });
    }

    // read helpers delegated to core
    QVector<Entity> readAll() const { return this->readAllImpl(); }
    const Entity* findById(const typename Entity::IdType& id) const { return this->findByIdImpl(id); }
};
