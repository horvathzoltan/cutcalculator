// products/view/product_tree_manager.cpp
#include "products/view/product_tree_manager.h"
#include <QStandardItem>

/**
 * Konstruktor: létrehozza a modellt és beállítja a QTreeView-hoz.
 */
ProductTreeManager::ProductTreeManager(QTreeView* view, QObject* parent)
    : QObject(parent), _view(view), _model(new QStandardItemModel(view))
{
    _model->setHorizontalHeaderLabels({ "Terméktípus", "Barcode", "Id" });
    _view->setModel(_model);
    _view->setHeaderHidden(false);
    _view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

/**
 * Teljes fa felépítése: gyökerek → rekurzívan gyerekek.
 */
void ProductTreeManager::populate() {
    _model->removeRows(0, _model->rowCount());

    auto roots = ProductRegistry::instance().roots();
    for (const auto& root : roots) {
        auto* nameItem = new QStandardItem(root.name);
        auto* codeItem = new QStandardItem(root.barcode);
        auto* idItem   = new QStandardItem(root.id.toString(QUuid::WithoutBraces));

        nameItem->setEditable(false);
        codeItem->setEditable(false);
        idItem->setEditable(false);

        _model->appendRow({ nameItem, codeItem, idItem });

        buildSubtree(nameItem, root.id);
    }

    _view->expandAll();
}

/**
 * Gyerekek felépítése parentId alapján.
 */
void ProductTreeManager::buildSubtree(QStandardItem* parentItem, const QUuid& parentId) {
    const auto children = ProductRegistry::instance().findChildren(parentId);
    for (const auto& child : children) {
        auto* nameItem = new QStandardItem(child.name);
        auto* codeItem = new QStandardItem(child.barcode);
        auto* idItem   = new QStandardItem(child.id.toString(QUuid::WithoutBraces));

        nameItem->setEditable(false);
        codeItem->setEditable(false);
        idItem->setEditable(false);

        parentItem->appendRow({ nameItem, codeItem, idItem });

        buildSubtree(nameItem, child.id);
    }
}
