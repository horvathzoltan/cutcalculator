// products/view/product_tree_view.cpp
#include "products/view/product_tree_view.h"
#include <QStandardItemModel>
#include <QDebug>
#include <QVBoxLayout>
#include "common/logger/event_logger.h"
#include "common/logger/logger.h"
#include "common/utils/font_utils.h"
#include "products/registry/product_registry.h"

ProductTreeView::ProductTreeView(QWidget* parent)
    : QTreeView(parent)
{
    FontUtils::applySafeMonospaceFont(this);

    // _layout = new QVBoxLayout(this);
    // _layout->setContentsMargins(0, 0, 0, 0);
    // _layout->setSpacing(0);

    // _layout->addWidget(this->viewport());   // a QTreeView saját viewportja

}

void ProductTreeView::dropEvent(QDropEvent* event) {
    auto* sim = qobject_cast<QStandardItemModel*>(model());
    if (!sim) {
        QTreeView::dropEvent(event);
        return;
    }

    QModelIndex droppedIndex = indexAt(event->position().toPoint());
    QUuid newParentId;

    if (droppedIndex.isValid()) {
        QString parentIdStr = sim->itemFromIndex(droppedIndex.sibling(droppedIndex.row(), 2))->text();
        newParentId = QUuid(parentIdStr);
    } else {
        newParentId = QUuid(); // gyökér
    }

    QModelIndex selected = currentIndex();
    if (!selected.isValid()) {
        QTreeView::dropEvent(event);
        return;
    }

    QString idStr = sim->itemFromIndex(selected.sibling(selected.row(), 2))->text();
    QUuid id(idStr);

    if (const ProductMaster* pm = ProductRegistry::instance().findById(id)) {
        ProductMaster updated = *pm;        // munka-másolat
        QUuid oldParent = updated.parentId; // régi parent
        updated.parentId = newParentId;     // új parent

        bool isUpdated = ProductRegistry::instance().update(updated);
        if (isUpdated){ // audit + persist
            ProductRegistry::instance().persist();

            zEventINFO(QString("🔀 Product moved: %1 → new parent: %2 (was: %3)")
                           .arg(updated.name)
                           .arg(newParentId.toString())
                           .arg(oldParent.toString()));
        }
        else{
            zWarning("⚠️ Product move failed (registry update rejected)");
        }
    }

    QTreeView::dropEvent(event); // alap drop kezelés

    emit productMoved(id, newParentId);
}




