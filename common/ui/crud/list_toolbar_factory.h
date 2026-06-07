#pragma once
#include "actionmap.h"

#include <QToolBar>
#include <QWidget>
#include <QAction>
#include <functional>
#include <vector>
#include <QString>

#include <ui/helpers/repository_overlay_widget.h>

// void OrderWorkbench::buildToolbar()
// {
//     _toolbar = new QToolBar("Order Actions", this);
//     _toolbar->setObjectName("order_toolbar");

//     _toolbar->addAction("🆕 Új rendelés");
//     _toolbar->addAction("💾 Mentés");
//     _toolbar->addAction("📂 Betöltés");
//     _toolbar->addAction("🗑️ Törlés");

// }


enum class ListAction {
    Add,
    Delete,
    Rename,
    Clone,
    MoveUp,
    MoveDown,
    Refresh
};

struct ListToolbarConfig {
    QWidget* parent = nullptr;

    std::vector<ListAction> actions;

    struct Callbacks {
        std::function<void()> onAdd;
        std::function<void()> onDelete;
        std::function<void()> onRename;
        std::function<void()> onClone;
        std::function<void()> onMoveUp;
        std::function<void()> onMoveDown;
        std::function<void()> onRefresh;
    } callbacks;

    QString labelPrefix;
    QString objectNamePrefix;
};

struct ListToolbarResult {
    QToolBar* toolbar = nullptr;
    QWidget* overlay = nullptr;

    ActionMap<ListAction> actionPointers;
};

class ListToolbarFactory {
private:
    static QAction* createAction(ListAction action, const ListToolbarConfig& cfg, QToolBar* tb)
    {


        if(action == ListAction::Add)
        {
            QString text = cfg.labelPrefix.isEmpty()
            ? QStringLiteral("➕ Új")
            : QStringLiteral("➕ Új %1").arg(cfg.labelPrefix);

            QAction* a = tb->addAction(text);
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_add");

            if (cfg.callbacks.onAdd)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onAdd]() { cb(); });

            return a;
        }

        if(action == ListAction::Delete)
        {
            QAction* a = tb->addAction(QStringLiteral("🗑️ Törlés"));
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_delete");

            if (cfg.callbacks.onDelete)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onDelete]() { cb(); });

            return a;
        }

        if(action == ListAction::Rename)
        {
            QAction* a = tb->addAction(QStringLiteral("✏️ Átnevezés"));
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_rename");

            if (cfg.callbacks.onRename)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onRename]() { cb(); });

            return a;
        }

        if(action ==  ListAction::Clone)
        {
            QAction* a = tb->addAction(QStringLiteral("📄 Másolat"));
            if (cfg.callbacks.onClone)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onClone]() { cb(); });
            return a;
        }

        if(action == ListAction::MoveUp)
        {
            QAction* a = tb->addAction(QStringLiteral("⬆️ Fel"));
            if (cfg.callbacks.onMoveUp)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onMoveUp]() { cb(); });
            return a;
        }

        if(action == ListAction::MoveDown)
        {
            QAction* a = tb->addAction(QStringLiteral("⬇️ Le"));
            if (cfg.callbacks.onMoveDown)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onMoveDown]() { cb(); });
            return a;
        }

        if(action == ListAction::Refresh)
        {
            QAction* a = tb->addAction(QStringLiteral("🔄 Frissítés"));
            if (cfg.callbacks.onRefresh)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onRefresh]() { cb(); });
            return a;
        }
        return nullptr;
    }

public:
template<typename RegistryT>
static ListToolbarResult create(const ListToolbarConfig& cfg)
    {
        ListToolbarResult result;

        auto* tb = new QToolBar(cfg.parent);
        result.toolbar = tb;

        // ⭐ OVERLAY
        auto* overlay = new RepositoryOverlayWidget<RegistryT>(
            tb,
            QStringLiteral("ListToolbarOverlay")
            );
        result.overlay = overlay;

        // ⭐ ACTIONS
        for (auto action : cfg.actions) {
            QAction* a = createAction(action, cfg, tb);
            result.actionPointers.insert(action, a);
        }

        return result;
    }

};

