#include "common/ui_state/widget_state_manager.h"
#include "common/ui_state/state_handlers.h"
#include "common/ui_state/widget_state_settings.h"
#include "common/logger/logger.h"
#include "common/utils/window_geometry_helper.h"
#include "widget_discovery_helper.h"

#include <QMetaObject>
#include <QMetaClassInfo>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QScrollBar>
#include <QTabBar>
#include <QAbstractButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>

#include <common/utils/filename_helper.h>

WidgetStateManager::WidgetStateManager(const QString& groupName)
    : _groupName(groupName)
{
    // Handler regisztrációk
    registerHandler<QSplitter>(
        StateHandlers::extractSplitter,
        StateHandlers::restoreSplitter);

    registerHandler<QHeaderView>(
        StateHandlers::extractHeader,
        StateHandlers::restoreHeader);

    registerHandler<QTabWidget>(
        StateHandlers::extractTabWidget,
        StateHandlers::restoreTabWidget);

    registerHandler<QAbstractScrollArea>(
        StateHandlers::extractScrollArea,
        StateHandlers::restoreScrollArea);
}

void WidgetStateManager::saveWidgetState(QWidget* root)
{
    if (!root) {
        zWarning().noquote() << "⚠️ [WidgetState] Save skipped → null root widget";
        return;
    }

    WidgetStateSettings s(_groupName);

    // TELJES SNAPSHOT TÖRLÉSE — PATCH 10
    s.clearUIState();

    const auto widgets = WidgetDiscoveryHelper::collect(root);

    zInfo().noquote()
        << QString("💾 [WidgetState] Saving (%1 widgets) → group=%2")
               .arg(widgets.size())
               .arg(_groupName);
    zInfo().noquote()
        << QString("💾 [WidgetState] Handler extract started → %1 widgets, group=%2")
               .arg(widgets.size())
               .arg(_groupName);


    // 1. Minden widget kap alap audit-információt
    for (QWidget* w : widgets) {
        const QString key = computeWidgetKey(w);

        s.map().insert(key + "/objectName", w->objectName());
        s.map().insert(key + "/class",
                       QString::fromLatin1(w->metaObject()->className()));
        s.map().insert(key + "/auditPath", computeWidgetPath(w));
        s.map().insert(key + "/state", QStringLiteral("unsaved"));
    }

    // 2. Handleres extract + handler-hiány jelölése
    for (QWidget* w : widgets) {
        const QString key = computeWidgetKey(w);

        const Handler* h = findHandler(w->metaObject());
        if (!h) {
            zWarning().noquote()
            << QString("⚠️ [WidgetState] No handler → class=%1 objectName=%2")
                    .arg(w->metaObject()->className())
                    .arg(w->objectName());

            if (!qobject_cast<QAbstractScrollArea*>(w) &&
                !qobject_cast<QSplitter*>(w) &&
                !qobject_cast<QHeaderView*>(w) &&
                !qobject_cast<QTabWidget*>(w)) {
                s.map().insert(key + "/handler", QStringLiteral("missing"));
            }
            continue;
        }

        h->extract(w, s.map(), key);

        zInfo().noquote()
            << QString("   ↳ extracted: %1").arg(key);
    }

    s.save();

    zInfo().noquote()
        << QString("📁 [WidgetState] State file written → %1 (group=%2)")
               .arg(filePath(), _groupName);
}

QString WidgetStateManager::filePath() const
{
    return FileNameHelper::instance().pathFor(
        FileKind::UIState_File,
        FileAccess::Write,
        _groupName
        );
}

void WidgetStateManager::restoreWidgetState(QWidget* root)
{
    if (!root) {
        zWarning().noquote() << "⚠️ [WidgetState] Restore skipped → null root widget";
        return;
    }

    if (!WindowGeometryHelper::isWindowGeometryReady(root)) {
        zInfo().noquote()
        << "⏳ [WidgetState] Geometry not ready → delayed restore (50 ms)";
        QTimer::singleShot(50, root, [this, root]() {
            this->restoreWidgetState(root);
        });
        return;
    }

    WidgetStateSettings s(_groupName);
    const auto widgets = WidgetDiscoveryHelper::collect(root);

    zInfo().noquote()
        << QString("🔁 [WidgetState] Restore started (%1 widgets) → group=%2")
               .arg(widgets.size())
               .arg(_groupName);

    zInfo().noquote()
        << QString("🔁 [WidgetState] Handler restore started → %1 widgets, group=%2")
               .arg(widgets.size())
               .arg(_groupName);


    for (QWidget* w : widgets) {
        const Handler* h = findHandler(w->metaObject());
        if (!h) {
            zWarning().noquote()
            << QString("⚠️ [WidgetState] No handler → class=%1 objectName=%2")
                    .arg(w->metaObject()->className())
                    .arg(w->objectName());
            continue;
        }

        const QString key = computeWidgetKey(w);
        h->restore(w, s.map(), key);

        zInfo().noquote()
            << QString("   ↳ restored: %1").arg(key);
    }

    zInfo().noquote()
        << QString("✅ [WidgetState] Restore finished → group=%1").arg(_groupName);
}


QString WidgetStateManager::computeWidgetPath(QWidget* w)
{
    QStringList parts;
    QWidget* p = w;
    while (p) {
        QString name = p->objectName();
        if (name.isEmpty())
            name = QString("unnamed_%1").arg(p->metaObject()->className());
        parts.prepend(name);
        p = p->parentWidget();
    }
    return parts.join("/");
}





QString WidgetStateManager::computeWidgetKey(QWidget* w) const
{
    QString name = w->objectName();
    if (name.isEmpty()) {
        const QByteArray className = w->metaObject()->className();
        QString ptr = QString("0x%1").arg(reinterpret_cast<quintptr>(w), 0, 16);

        name = QStringLiteral("unnamed_%1_%2")
                   .arg(QString::fromLatin1(className))
                   .arg(ptr);

        zWarning().noquote()
            << "⚠️ WidgetStateManager: unnamed layout-critical widget → using fallback key"
            << className
            << ptr;
    }

    return _groupName + "/" + name;
}


const WidgetStateManager::Handler* WidgetStateManager::findHandler(const QMetaObject* mo) const
{
    while (mo) {
        const QByteArray className = mo->className();
        auto it = _handlers.constFind(className);
        if (it != _handlers.constEnd())
            return &(*it);

        mo = mo->superClass();
    }
    return nullptr;
}
