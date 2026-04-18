#pragma once

#include <QWidget>
#include <QHash>
#include <QSettings>
#include <functional>

#include <QSplitter>
#include <QHeaderView>
#include <QTabWidget>
#include <QAbstractScrollArea>

class UIStateCollector
{
public:
    explicit UIStateCollector(const QString& groupName);

    void collect(QWidget* root);
    void restore(QWidget* root);

private:
    QString _groupName;

    using ExtractFn = std::function<void(QWidget*, QVariantMap&, const QString&)>;
    using RestoreFn = std::function<void(QWidget*, QVariantMap&, const QString&)>;

    struct Handler {
        ExtractFn extract;
        RestoreFn restore;
    };

    QHash<QByteArray, Handler> _handlers;

    QString keyFor(QWidget* w) const;

    template<typename T>
    void registerHandler(ExtractFn e, RestoreFn r)
    {
        _handlers.insert(T::staticMetaObject.className(), Handler{e, r});
    }

    const Handler* findHandler(const QMetaObject* mo) const;
};
