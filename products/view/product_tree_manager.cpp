// products/view/product_tree_manager.cpp
#include "products/view/product_tree_manager.h"
#include "common/logger/event_logger.h"
#include <QStandardItem>
#include "products/registry/product_registry.h"

/**
 * Konstruktor: létrehozza a modellt és beállítja a QTreeView-hoz.
 */
ProductTreeManager::ProductTreeManager(QTreeView* view, QObject* parent)
    : QObject(parent), _view(view), _model(new QStandardItemModel(view))
{
    _model->setHorizontalHeaderLabels({ "Terméktípus", "Barcode", "Id" });
    _view->setModel(_model);
    _view->setHeaderHidden(false);
    //_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    _view->setDragDropMode(QAbstractItemView::InternalMove);
    _view->setDefaultDropAction(Qt::MoveAction);

    connect(_model, &QStandardItemModel::itemChanged,
            this, &ProductTreeManager::onItemChanged);
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

        nameItem->setEditable(true);
        codeItem->setEditable(true);
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

        nameItem->setEditable(true);
        codeItem->setEditable(true);
        idItem->setEditable(false);

        parentItem->appendRow({ nameItem, codeItem, idItem });

        buildSubtree(nameItem, child.id);
    }
}

// CRUD

void ProductTreeManager::addRootProduct() {
    ProductMaster pm;
    pm.id = QUuid::createUuid();
    pm.parentId = QUuid(); // gyökér
    pm.name = "Új termék";
    pm.barcode = "NEW";
    ProductRegistry::instance().insert(pm);
    populate();
}

void ProductTreeManager::addChildProduct() {
    auto index = _view->currentIndex();
    if (!index.isValid()) return;

    QString parentIdStr = _model->itemFromIndex(index.sibling(index.row(), 2))->text();
    QUuid parentId(parentIdStr);

    ProductMaster pm;
    pm.id = QUuid::createUuid();
    pm.parentId = parentId;
    pm.name = "Új gyermek";
    pm.barcode = "NEWCHILD";
   ProductRegistry::instance().insert(pm);
    populate();
}

void ProductTreeManager::renameProduct() {
    auto index = _view->currentIndex();
    if (!index.isValid()) return;
    _view->edit(index.sibling(index.row(), 0)); // name oszlop szerkesztése
}

void ProductTreeManager::removeProduct() {
    auto index = _view->currentIndex();
    if (!index.isValid()) return;

    QString idStr = _model->itemFromIndex(index.sibling(index.row(), 2))->text();
    QUuid id(idStr);

    ProductRegistry::instance().remove(id);
    populate();
}

void ProductTreeManager::onItemChanged(QStandardItem* item) {
    //QString idStr = _model->item(item->row(), 2)->text();
    auto index = item->index();
    QString idStr = _model->itemFromIndex(index.sibling(index.row(), 2))->text();

    QUuid id(idStr);

    if (auto* pm = ProductRegistry::instance().findById(id)) {
        if (item->column() == 0) { // name
            pm->name = item->text();
            zEvent(QString("✏️ Product renamed: %1").arg(item->text()));
        }
        else if (item->column() == 1) { // barcode
            QString newCode = item->text();

            // Validálás: nem lehet üres
            if (newCode.isEmpty()) {
                zEvent("⚠️ Barcode nem lehet üres");
                item->setText(pm->barcode); // visszaállítjuk
                return;
            }

            // Validálás: egyediség
            if (!ProductRegistry::instance().isBarcodeUnique(newCode, pm->id)) {
                zEvent("⚠️ Barcode nem egyedi");
                item->setText(pm->barcode);
                return;
            }

            // Ha minden rendben → registry frissítése
            pm->barcode = newCode;
            zEvent(QString("✏️ Product barcode updated: %1").arg(newCode));
        }

        ProductRegistry::instance().update(*pm);
    }
}
