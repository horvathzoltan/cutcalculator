#pragma once

#include <QToolBar>
#include <QWidget>
#include <QAction>
#include <functional>
#include <vector>

#include "ui/helpers/repository_overlay_widget.h"

// ------------------------------------------------------------
// CRUD enumok
// ------------------------------------------------------------
enum class CrudAction {
    Add,
    Delete,
    Rename,
    Clone,
    MoveUp,
    MoveDown
};

enum class CrudOverlay {
    None,
    Enabled
};

// ------------------------------------------------------------
// Konfig struktúra
// ------------------------------------------------------------
struct CrudToolbarConfig {
    QWidget* parent = nullptr;
    QWidget* view   = nullptr;   // opcionális, ha később kell

    std::vector<CrudAction> actions;

    struct Callbacks {
        std::function<void()> onAdd;
        std::function<void()> onDelete;
        std::function<void()> onRename;
        std::function<void()> onClone;
        std::function<void()> onMoveUp;
        std::function<void()> onMoveDown;
    } callbacks;

    CrudOverlay overlay = CrudOverlay::None;
};

// ------------------------------------------------------------
// Factory – template RegistryT‑re
// ------------------------------------------------------------
class CrudToolbarFactory {
public:
    template<typename RegistryT>
    static QToolBar* create(const CrudToolbarConfig& cfg)
    {
        auto* tb = new QToolBar(cfg.parent);

        QAction* addAct    = nullptr;
        QAction* delAct    = nullptr;
        QAction* renAct    = nullptr;
        QAction* cloneAct  = nullptr;
        QAction* upAct     = nullptr;
        QAction* downAct   = nullptr;

        for (auto action : cfg.actions) {
            switch (action) {
            case CrudAction::Add:
                addAct = tb->addAction(QStringLiteral("➕ Új"));
                if (cfg.callbacks.onAdd) {
                    QObject::connect(addAct, &QAction::triggered,
                                     tb, [cb = cfg.callbacks.onAdd]() { cb(); });
                }
                break;

            case CrudAction::Delete:
                delAct = tb->addAction(QStringLiteral("🗑️ Törlés"));
                if (cfg.callbacks.onDelete) {
                    QObject::connect(delAct, &QAction::triggered,
                                     tb, [cb = cfg.callbacks.onDelete]() { cb(); });
                }
                break;

            case CrudAction::Rename:
                renAct = tb->addAction(QStringLiteral("✏️ Átnevezés"));
                if (cfg.callbacks.onRename) {
                    QObject::connect(renAct, &QAction::triggered,
                                     tb, [cb = cfg.callbacks.onRename]() { cb(); });
                }
                break;

            case CrudAction::Clone:
                cloneAct = tb->addAction(QStringLiteral("📄 Másolat"));
                if (cfg.callbacks.onClone) {
                    QObject::connect(cloneAct, &QAction::triggered,
                                     tb, [cb = cfg.callbacks.onClone]() { cb(); });
                }
                break;

            case CrudAction::MoveUp:
                upAct = tb->addAction(QStringLiteral("⬆️ Fel"));
                if (cfg.callbacks.onMoveUp) {
                    QObject::connect(upAct, &QAction::triggered,
                                     tb, [cb = cfg.callbacks.onMoveUp]() { cb(); });
                }
                break;

            case CrudAction::MoveDown:
                downAct = tb->addAction(QStringLiteral("⬇️ Le"));
                if (cfg.callbacks.onMoveDown) {
                    QObject::connect(downAct, &QAction::triggered,
                                     tb, [cb = cfg.callbacks.onMoveDown]() { cb(); });
                }
                break;
            }
        }

        // Opcionális overlay – RegistryT::instance().size() alapján
        if (cfg.overlay == CrudOverlay::Enabled) {
            // A widget magát hozzáadja a toolbarhoz a konstruktorban
            new RepositoryOverlayWidget<RegistryT>(tb, QStringLiteral("CrudToolbarOverlay"));
        }

        return tb;
    }
};
