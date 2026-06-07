#pragma once
#include "actionmap.h"

#include <QToolBar>
#include <QWidget>
#include <QAction>
#include <functional>
#include <vector>
#include <QString>

#include <ui/helpers/repository_overlay_widget.h>

enum class EntityAction {
    Modify,
    Save,
    Cancel,
    Delete,
    Add
};

enum class EntityOverlay {
    None,
    Enabled
};

struct EntityToolbarConfig {
    QWidget* parent = nullptr;
    std::vector<EntityAction> actions;
    EntityOverlay overlay = EntityOverlay::None;

    struct Callbacks {
        std::function<void()> onModify;
        std::function<void()> onSave;
        std::function<void()> onCancel;
        std::function<void()> onDelete;
        std::function<void()> onAdd;
    } callbacks;

    QString labelPrefix;
    QString objectNamePrefix;
};

struct EntityToolbarResult {
    QToolBar* toolbar;
    QWidget* overlay;

    ActionMap<EntityAction> actionPointers;
};

class EntityToolbarFactory {   
private:
    static QAction* createAction(EntityAction action, const EntityToolbarConfig& cfg, QToolBar* tb)
    {
        if(action ==  EntityAction::Modify)
        {
            QAction* a = tb->addAction(QStringLiteral("✏️ Szerkesztés"));
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_modify");

            if (cfg.callbacks.onModify)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onModify]() { cb(); });

            return a;;
        }

        if(action ==  EntityAction::Save)
        {
            QAction* a = tb->addAction(QStringLiteral("💾 Mentés"));
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_save");

            if (cfg.callbacks.onSave)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onSave]() { cb(); });

            return a;;
        }

        if(action ==  EntityAction::Cancel)
        {
            QAction* a = tb->addAction(QStringLiteral("✖ Mégse"));
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_cancel");

            if (cfg.callbacks.onCancel)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onCancel]() { cb(); });

            return a;;
        }

        if(action ==  EntityAction::Delete)
        {
            QAction* a = tb->addAction(QStringLiteral("🗑️ Törlés"));
            if (!cfg.objectNamePrefix.isEmpty())
                a->setObjectName(cfg.objectNamePrefix + "_delete");

            if (cfg.callbacks.onDelete)
                QObject::connect(a, &QAction::triggered, tb, [cb = cfg.callbacks.onDelete]() { cb(); });

            return a;;
        }

        if(action ==  EntityAction::Add)
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
        return nullptr;
    }

public:

template<typename RegistryT>
static EntityToolbarResult create(const EntityToolbarConfig& cfg)
    {
        EntityToolbarResult result;

        auto* tb = new QToolBar(cfg.parent);
        result.toolbar = tb;

        // ⭐ OVERLAY (opcionális)
        RepositoryOverlayWidget<RegistryT>* overlay = nullptr;

        if (cfg.overlay == EntityOverlay::Enabled) {
            overlay = new RepositoryOverlayWidget<RegistryT>(
                tb,
                QStringLiteral("EntityToolbarOverlay")
                );
        }

        result.overlay = overlay;

        // ⭐ ACTIONS
        for (auto action : cfg.actions) {
            QAction* a = createAction(action, cfg, tb);
            result.actionPointers.insert(action, a);
        }

        return result;
    }

};
