#include "common/ui_state/widget_state_settings.h"
#include "common/logger/logger.h"
#include <common/window_state/window_state_manager.h>


/*
 * Konstruktor
 * -----------
 * Betölti a groupName-hez tartozó widget state map-et a WindowStateManager-ből.
 * A WidgetStateManager ezután módosítja, majd save() visszamenti.
 */
WidgetStateSettings::WidgetStateSettings(const QString& groupName)
    : _groupName(groupName)
    , _map(WindowStateManager::instance().loadWidgetState(groupName))
{
    // opcionális log:
    // zInfo().noquote() << "📥 [WidgetState] Loaded map for group=" << groupName;
}

QVariantMap& WidgetStateSettings::map()
{
    return _map;
}

const QVariantMap& WidgetStateSettings::map() const
{
    return _map;
}

QString WidgetStateSettings::prefix() const
{
    return _groupName;
}

/*
 * save()
 * ------
 * A módosított QVariantMap-et visszamenti a WindowStateManager-be.
 */
void WidgetStateSettings::save() const
{
    WindowStateManager::instance().saveWidgetState(_groupName, _map);

    // opcionális log:
    zInfo().noquote() << "💾 [WidgetState] Saved map → group=" << _groupName;
}

/*
 * clearUIState()
 * --------------
 * Teljes UIState törlése a WindowStateManager-ben,
 * majd az in-memory map kiürítése.
 */
void WidgetStateSettings::clearUIState()
{
    WindowStateManager::instance().clearWidgetState(_groupName);
    _map.clear();

    // opcionális log:
    zInfo().noquote() << "🧹 [WidgetState] Cleared UIState → group=" << _groupName;
}



