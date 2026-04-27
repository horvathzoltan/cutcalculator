#include "workbench_state_manager.h"
#include "common/logger/logger.h"
#include "common/ui_state/widget_state_manager.h"
#include <QCoreApplication>
#include <QTimer>
#include <QWidget>

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
        WidgetStateManager c(id);
        c.restoreWidgetState(wb);
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
