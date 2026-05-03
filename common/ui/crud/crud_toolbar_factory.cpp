// #include "crud_toolbar_factory.h"

// #include <QObject>

// // ha van ilyen:
// #include "ui/helpers/repository_overlay_widget.h"
// #include "common/registry/base/registry_engine_base.h"   // AbstractRegistry, ha itt van

// QToolBar* CrudToolbarFactory::create(const CrudToolbarConfig& cfg)
// {
//     auto* tb = new QToolBar(cfg.parent);

//     QAction* addAct    = nullptr;
//     QAction* delAct    = nullptr;
//     QAction* renAct    = nullptr;
//     QAction* cloneAct  = nullptr;
//     QAction* upAct     = nullptr;
//     QAction* downAct   = nullptr;

//     for (auto action : cfg.actions) {
//         switch (action) {
//         case CrudAction::Add:
//             addAct = tb->addAction(QStringLiteral("➕ Új"));
//             if (cfg.callbacks.onAdd) {
//                 QObject::connect(addAct, &QAction::triggered,
//                                  tb, [cb = cfg.callbacks.onAdd]() { cb(); });
//             }
//             break;

//         case CrudAction::Delete:
//             delAct = tb->addAction(QStringLiteral("🗑️ Törlés"));
//             if (cfg.callbacks.onDelete) {
//                 QObject::connect(delAct, &QAction::triggered,
//                                  tb, [cb = cfg.callbacks.onDelete]() { cb(); });
//             }
//             break;

//         case CrudAction::Rename:
//             renAct = tb->addAction(QStringLiteral("✏️ Átnevezés"));
//             if (cfg.callbacks.onRename) {
//                 QObject::connect(renAct, &QAction::triggered,
//                                  tb, [cb = cfg.callbacks.onRename]() { cb(); });
//             }
//             break;

//         case CrudAction::Clone:
//             cloneAct = tb->addAction(QStringLiteral("📄 Másolat"));
//             if (cfg.callbacks.onClone) {
//                 QObject::connect(cloneAct, &QAction::triggered,
//                                  tb, [cb = cfg.callbacks.onClone]() { cb(); });
//             }
//             break;

//         case CrudAction::MoveUp:
//             upAct = tb->addAction(QStringLiteral("⬆️ Fel"));
//             if (cfg.callbacks.onMoveUp) {
//                 QObject::connect(upAct, &QAction::triggered,
//                                  tb, [cb = cfg.callbacks.onMoveUp]() { cb(); });
//             }
//             break;

//         case CrudAction::MoveDown:
//             downAct = tb->addAction(QStringLiteral("⬇️ Le"));
//             if (cfg.callbacks.onMoveDown) {
//                 QObject::connect(downAct, &QAction::triggered,
//                                  tb, [cb = cfg.callbacks.onMoveDown]() { cb(); });
//             }
//             break;
//         }
//     }

//     // Opcionális overlay – ha van registry + overlay infra
//     if (cfg.overlay == CrudOverlay::Enabled && cfg.registry) {
//         new RepositoryOverlayWidget(cfg.registry, tb, "CrudToolbarOverlay");
//     }

//     return tb;
// }
