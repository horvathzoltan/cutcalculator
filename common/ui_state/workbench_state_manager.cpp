#include "widget_state_settings.h"
#include "workbench_state_manager.h"
#include "common/logger/logger.h"
#include "common/ui_state/widget_state_manager.h"
#include <QCoreApplication>
#include <QTimer>
#include <QWidget>
#include <common/window_state/window_state_manager.h>

WorkbenchStateManager& WorkbenchStateManager::instance() {
    static WorkbenchStateManager inst;
    return inst;
}

WorkbenchStateManager::WorkbenchStateManager(QObject* parent)
    : QObject(parent) {}

void WorkbenchStateManager::registerWorkbench(const QString& id, QWidget* wb) {
    _workbenches[id] = { wb, false };
}


// workbench_state_manager.cpp
void WorkbenchStateManager::onTabActivated(QWidget* wb) {
    if (_closing)
        return; // zárás közben nem restaurálunk

    QString id = findIdByWidget(wb);
    if (id.isEmpty())
        return;

    zInfo().noquote() << "🔁 [WorkbenchState] Tab activated → id=" << id;

    auto& info = _workbenches[id];
    if (!info.wasShown) {

        // 1) Mindig restore
        WidgetStateManager c(id);
        c.restoreWidgetState(wb);

        // 2) Ha van postRestoreFix, meghívjuk (NEM skip-el dönt)
        int methodIndex = wb->metaObject()->indexOfMethod("postRestoreFix()");
        if (methodIndex >= 0) {
            QMetaObject::invokeMethod(wb, "postRestoreFix");
        }

        info.wasShown = true;
    }
}

bool WorkbenchStateManager::isShown(QWidget* wb){
    QString id = findIdByWidget(wb);
    if (id.isEmpty()) {
        zWarning().noquote() << "⚠️ [WorkbenchState] onTabDeactivated: widget not registered, skipping save";
        return false;
    }

    auto& info = _workbenches[id];
    return info.wasShown;
}

void WorkbenchStateManager::onTabDeactivated(QWidget* wb, bool immediate)
{
    QString id = findIdByWidget(wb);
    if (id.isEmpty()) {
        zWarning().noquote() << "⚠️ [WorkbenchState] onTabDeactivated: widget not registered, skipping save";
        return;
    }

    auto& info = _workbenches[id];
    if (!info.wasShown)
        return; // sosem volt megnyitva → nincs mit menteni

    auto doSave = [id, wb]() {
        WidgetStateManager c(id);
        c.saveWidgetState(wb);
        zInfo().noquote() << "💾 [WorkbenchState] Saved snapshot on tab leave → id=" << id;
    };

    if (immediate || _closing) {
        doSave();        // kilépéskor / explicit kérésre: szinkron
    } else {
        QTimer::singleShot(0, wb, doSave); // normál tabváltáskor: aszinkron
    }
}

void WorkbenchStateManager::beginClosing() {
    _closing = true;
}


bool isSameOrParent(QWidget* parent, QWidget* child) {
    QWidget* w = child;
    while (w) {
        if (w == parent)
            return true;
        w = w->parentWidget();
    }
    return false;
}

QString WorkbenchStateManager::findIdByWidget(QWidget* wb) const
{
    for (auto it = _workbenches.begin(); it != _workbenches.end(); ++it) {
        if (isSameOrParent(it.value().widget, wb))
            return it.key();
    }
    return {};
}

QVariantMap WorkbenchStateManager::loadCustomState(const QString& id)
{
    QVariantMap result;

    WidgetStateSettings s(id);
    const QVariantMap& map = s.map();

    const QString prefix = "customState/";

    for (auto it = map.begin(); it != map.end(); ++it) {
        if (it.key().startsWith(prefix)) {
            const QString subKey = it.key().mid(prefix.size());
            result.insert(subKey, it.value());
        }
    }

    return result;
}

void WorkbenchStateManager::saveCustomState(const QString& id, const QVariantMap& state)
{
    WidgetStateSettings s(id);
    QVariantMap& map = s.map();

    const QString prefix = "customState/";

    // előző customState törlése
    QStringList toRemove;
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (it.key().startsWith(prefix))
            toRemove << it.key();
    }
    for (const QString& k : toRemove)
        map.remove(k);

    // új értékek írása
    for (auto it = state.begin(); it != state.end(); ++it) {
        map.insert(prefix + it.key(), it.value());
    }

    s.save();
}

