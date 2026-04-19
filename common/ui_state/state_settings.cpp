#include "common/ui_state/state_settings.h"
#include "common/snapshot/snapshot_manager.h"

StateSettings::StateSettings(const QString& groupName)
    : _groupName(groupName)
    , _map(SnapshotManager::instance().loadUIState(groupName))
{
}

QVariantMap& StateSettings::map()
{
    return _map;
}

const QVariantMap& StateSettings::map() const
{
    return _map;
}

QString StateSettings::prefix() const
{
    return _groupName;
}

void StateSettings::save() const
{
    SnapshotManager::instance().saveUIState(_groupName, _map);
}

void StateSettings::clearUIState()
{
    SnapshotManager::instance().clearUIState(_groupName);
    _map.clear();
}



