#pragma once
#include <QVariantMap>

/**
 * @brief Interface for workbenches that want to save/restore custom (non-UI) state.
 *
 * This interface is intentionally minimal:
 *  - saveCustomState(): returns a QVariantMap with key/value pairs to persist
 *  - restoreCustomState(): receives the previously saved map
 *
 * The WorkbenchStateManager will:
 *  - detect if a workbench implements this interface (via qobject_cast)
 *  - call saveCustomState() on tab deactivation
 *  - call restoreCustomState() on first tab activation
 *
 * IMPORTANT:
 *  - This is NOT for UI geometry (that is handled by WidgetStateManager)
 *  - This is for logical state (e.g. selected entity ID, filters, search text)
 *  - restoreCustomState() should block signals internally (QSignalBlocker)
 */
class IWorkbenchCustomState
{
public:
    virtual ~IWorkbenchCustomState() = default;

    virtual QVariantMap saveCustomState() const = 0;
    virtual void restoreCustomState(const QVariantMap& state) = 0;
};
