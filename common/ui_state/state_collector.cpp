#include "common/ui_state/state_collector.h"
#include "common/ui_state/state_handlers.h"
#include "common/ui_state/state_settings.h"

#include <QMetaObject>
#include <QMetaClassInfo>

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
    const auto widgets = collectWidgets(root);

    for (QWidget* w : widgets) {
        const QByteArray className = w->metaObject()->className();
        if (!_handlers.contains(className))
            continue;

        const QString key = keyFor(w);
        _handlers[className].extract(w, s.map(), key);
    }

    s.save();

}

void UIStateCollector::restore(QWidget* root)
{
    if (!root) return;

    StateSettings s(_groupName);
    const auto widgets = collectWidgets(root);

    for (QWidget* w : widgets) {
        const QByteArray className = w->metaObject()->className();
        if (!_handlers.contains(className))
            continue;

        const QString key = keyFor(w);
        _handlers[className].restore(w, s.map(), key);
    }

}

QList<QWidget*> UIStateCollector::collectWidgets(QWidget* root) const
{
    QList<QWidget*> list;
    if (!root)
        return list;

    list << root;

    const auto children = root->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget* c : children)
        list << collectWidgets(c);

    return list;
}

QString UIStateCollector::keyFor(QWidget* w) const
{
    QString name = w->objectName();
    if (name.isEmpty())
        name = QString::fromLatin1(w->metaObject()->className());

    return _groupName + "/" +
           QString::fromLatin1(w->metaObject()->className()) +
           "/" + name;
}
