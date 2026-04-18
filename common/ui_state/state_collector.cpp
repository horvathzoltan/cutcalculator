#include "common/ui_state/state_collector.h"
#include "common/ui_state/state_handlers.h"
#include "common/ui_state/state_settings.h"
#include "common/logger/logger.h"
#include "layout_critical_helper.h"

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


UIStateCollector::UIStateCollector(const QString& groupName)
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

void UIStateCollector::collect(QWidget* root)
{
    if (!root) return;

    StateSettings s(_groupName);
    const auto widgets = LayoutCriticalHelper::collect(root);

    // 1. Minden widget kap alap audit-információt
    for (QWidget* w : widgets) {
        const QString key = keyFor(w);
        s.map().insert(key + "/state", QStringLiteral("unsaved"));
        s.map().insert(key + "/class",
                       QString::fromLatin1(w->metaObject()->className()));
        s.map().insert(key + "/objectName", w->objectName());
    }

    // 2. Handleres extract + handler-hiány jelölése
    for (QWidget* w : widgets) {
        const QString key = keyFor(w);

        const Handler* h = findHandler(w->metaObject());
        if (!h) {
            zWarning().noquote()
            << "⚠️ UIStateCollector: no handler for"
            << w->metaObject()->className()
            << "(objectName=" << w->objectName() << ")";

            s.map().insert(key + "/handler", QStringLiteral("missing"));
            continue;
        }

        h->extract(w, s.map(), key);
    }

    s.save();
}


void UIStateCollector::restore(QWidget* root)
{
    if (!root) return;

    StateSettings s(_groupName);
    const auto widgets = LayoutCriticalHelper::collect(root);

    for (QWidget* w : widgets) {
        const Handler* h = findHandler(w->metaObject());
        if (!h) {
            zWarning().noquote()
            << "⚠️ UIStateCollector: no handler for"
            << w->metaObject()->className()
            << "(objectName=" << w->objectName() << ")";
            continue;
        }

        const QString key = keyFor(w);
        h->restore(w, s.map(), key);
    }

}


QString UIStateCollector::keyFor(QWidget* w) const
{
    QString name = w->objectName();
    if (name.isEmpty()) {
        const QByteArray className = w->metaObject()->className();
        QString ptr = QString("0x%1").arg(reinterpret_cast<quintptr>(w), 0, 16);

        name = QStringLiteral("unnamed_%1_%2")
                   .arg(QString::fromLatin1(className))
                   .arg(ptr);

        zWarning().noquote()
            << "⚠️ UIStateCollector: unnamed layout-critical widget → using fallback key"
            << className
            << ptr;
    }

    return _groupName + "/" + name;
}


const UIStateCollector::Handler* UIStateCollector::findHandler(const QMetaObject* mo) const
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
