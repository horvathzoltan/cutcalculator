// products/view/product_tree_view.cpp
#include "products/view/product_tree_view.h"
#include <QStandardItemModel>
#include <QDebug>
#include "common/logger/event_logger.h"
#include "products/registry/product_registry.h"

void ProductTreeView::dropEvent(QDropEvent* event) {
    auto* sim = qobject_cast<QStandardItemModel*>(model());
    if (!sim) {
        QTreeView::dropEvent(event);
        return;
    }

    QModelIndex droppedIndex = indexAt(event->position().toPoint());
    if (!droppedIndex.isValid()) {
        QTreeView::dropEvent(event);
        return;
    }

    // Új parent ID
    QString parentIdStr = sim->itemFromIndex(droppedIndex.sibling(droppedIndex.row(), 2))->text();
    QUuid newParentId(parentIdStr);

    // Mozgatott elem ID
    QModelIndex selected = currentIndex();
    QString idStr = sim->itemFromIndex(selected.sibling(selected.row(), 2))->text();
    QUuid id(idStr);

    if (auto* pm = ProductRegistry::instance().findById(id)) {
        QUuid oldParent = pm->parentId;
        pm->parentId = newParentId;
        ProductRegistry::instance().update(*pm);

        zEventINFO(QString("🔀 Product moved: %1 → new parent: %2 (was: %3)")
                       .arg(pm->name)
                       .arg(newParentId.toString())
                       .arg(oldParent.toString()));
    }

    QTreeView::dropEvent(event);
}
