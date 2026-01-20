#include "products/presenter/product_tree_presenter.h"

#include <products/registry/product_registry.h>

ProductTreePresenter::ProductTreePresenter(ProductTreeView* view,
                                           ProductTreeManager* manager,
                                           QObject* parent)
    : QObject(parent)
    , _manager(manager)
{
    _view = view;
}

QToolBar* ProductTreePresenter::buildToolbar(QWidget* parent)
{
    auto* tb = new QToolBar("Termékfa", parent);

    _status = new RepositoryOverlayWidget<ProductRegistry>(tb, "ProductTreeOverlay");

    refreshOverlayOnly();
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

void ProductTreePresenter::refreshOverlayOnly()
{
    int visible = _view->model() ? _view->model()->rowCount() : 0;
    _status->refresh(visible);
}

void ProductTreePresenter::connectRegistry()
{
    ProductRegistry::instance().subscribeItemsChangedToken(
        [this]() { refreshOverlayOnly(); });
}

void ProductTreePresenter::connectTreeStats()
{
    QObject::connect(_manager, &ProductTreeManager::treeStatsChanged,
                     this, [this]() { refreshOverlayOnly(); });
}
