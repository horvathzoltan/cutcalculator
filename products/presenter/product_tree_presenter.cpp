#include "products/presenter/product_tree_presenter.h"

#include <products/registry/product_registry.h>

ProductTreePresenter::ProductTreePresenter(ProductTreePanel* panel,
                                           ProductTreeManager* manager,
                                           QObject* parent)
    : QObject(parent)
    , _panel(panel)
    , _manager(manager)
{
    _view = panel->tree(); // a TreeView innen jön
}

QToolBar* ProductTreePresenter::buildToolbar(QWidget* parent)
{
    auto* tb = new QToolBar("Termékfa", parent);

    _status = new OverlayIconWidget();
    //_status->setBaseEmoji("🌳");
    _status->setBaseEmoji("📄");
    tb->addWidget(_status);
    //_panel->setStatusWidget(_status);

    initialOverlay();
    connectRegistry();
    connectTreeStats();


    QAction* addRootAct   = tb->addAction("➕ Új termékcsoport");
    QAction* addChildAct  = tb->addAction("➕ Új terméktípus");
    QAction* renameAct    = tb->addAction("✏️ Átnevezés");
    QAction* removeAct    = tb->addAction("🗑️ Törlés");

    QObject::connect(addRootAct,  &QAction::triggered, _manager, &ProductTreeManager::addRootProduct);
    QObject::connect(addChildAct, &QAction::triggered, _manager, &ProductTreeManager::addChildProduct);
    QObject::connect(renameAct,   &QAction::triggered, _manager, &ProductTreeManager::renameProduct);
    QObject::connect(removeAct,   &QAction::triggered, _manager, &ProductTreeManager::removeProduct);

    return tb;
}

void ProductTreePresenter::initialOverlay()
{
    int repo = ProductRegistry::instance().size();
    int visible = _view->model() ? _view->model()->rowCount() : 0;
    _status->updateOverlayState2(repo, visible);
}

void ProductTreePresenter::connectRegistry()
{
    ProductRegistry::instance().subscribeItemsChangedToken([this]() {
        int repo = ProductRegistry::instance().size();
        int visible = _view->model() ? _view->model()->rowCount() : 0;
        _status->updateOverlayState2(repo, visible);
    });
}

void ProductTreePresenter::connectTreeStats()
{
    QObject::connect(_manager, &ProductTreeManager::treeStatsChanged,
                     this, [this](int repoCount, int visibleRows) {
                         _status->updateOverlayState2(repoCount, visibleRows);
                     });
}
