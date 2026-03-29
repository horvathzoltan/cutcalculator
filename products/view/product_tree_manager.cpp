// products/view/product_tree_manager.cpp
#include "products/view/product_tree_manager.h"
#include "common/csv/filecontext.h"
#include "ui/style/color_helper.h"
#include "common/logger/event_logger.h"
#include <QStandardItem>
#include "products/registry/product_registry.h"
#include "products/view/product_tree_view.h"
#include "common/logger/logger.h"
#include <barcodes/validator/barcode_validator.h>
#include <barcodes/helpers/barcode_generator.h>

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

    if (auto* ptv = qobject_cast<ProductTreeView*>(_view)) {
        connect(ptv, &ProductTreeView::productMoved,
                this, &ProductTreeManager::onProductMoved);
    }

    connect(_view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ProductTreeManager::onCurrentChanged);

}

/**
 * Stílus beállítása Termékcsoport (ág) és Terméktípus (levél) szerint.
 * - Termékcsoport: félkövér + ciánkék
 * - Terméktípus: dőlt + fehér
 */
void ProductTreeManager::styleItem(QStandardItem* item, bool isLeaf, bool isRoot) {
    QFont font = item->font();
    if (isRoot) {
        font.setBold(true);
        item->setFont(font);
        item->setForeground(QBrush(Qt::darkGreen)); // barátságosabb szín
        if (!item->text().startsWith("🌳 ")) {
            item->setText("🌳 " + item->text()); // gyökér → házikó ikon
        }
    }
    else if (isLeaf) {
        font.setItalic(true);
        font.setBold(false);
        item->setFont(font);
        item->setForeground(QBrush(ColorHelper::textColor(_view)));
        if (item->text().startsWith("📂 ")) {
            item->setText(item->text().mid(3)); // levágja a 📂‑t
        }
    }
    else {
        font.setBold(true);
        font.setItalic(false);
        item->setFont(font);
        item->setForeground(QBrush(ColorHelper::categoryColor(_view)));
        if (!item->text().startsWith("📂 ")) {
            item->setText("📂 " + item->text());
        }
    }
}


/**
 * Teljes fa felépítése: gyökerek → rekurzívan gyerekek.
 * A gyökér elemeket Termékcsoportként jelenítjük meg.
 */
void ProductTreeManager::populate() {
    _model->removeRows(0, _model->rowCount());

    auto roots = ProductRegistry::instance().findRoots();
    for (const auto& root : roots) {
        auto* nameItem = new QStandardItem(root.name);
        auto* codeItem = new QStandardItem(root.barcode);
        auto* idItem   = new QStandardItem(root.id.toString(QUuid::WithoutBraces));

        nameItem->setEditable(true);
        codeItem->setEditable(true);
        idItem->setEditable(false);

        // Gyökér mindig root
        styleItem(nameItem, /*isLeaf=*/false, /*isRoot=*/true);

        _model->appendRow({ nameItem, codeItem, idItem });

        buildSubtree(nameItem, root.id);
    }

    _view->expandAll();

    if (_view && _model)
        emit treeStatsChanged(ProductRegistry::instance().size(),
                              _model->rowCount());
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

        // Eldöntjük, hogy levél-e (nincsenek gyerekei)
        bool isLeaf = ProductRegistry::instance().findChildren(child.id).isEmpty();
        styleItem(nameItem, isLeaf, /*isRoot=*/false);


        parentItem->appendRow({ nameItem, codeItem, idItem });

        buildSubtree(nameItem, child.id);
    }
}

// CRUD

void ProductTreeManager::addRootProduct() {
    ProductMaster pm;
    pm.id = QUuid::createUuid();
    pm.parentId = QUuid(); // gyökér
    pm.name = "Új gyökérelem";
    pm.barcode = BarcodeGenerator::generate("PROD", pm.name, 6);

    //ProductRegistry::instance().insert(pm);
    if (!ProductRegistry::instance().insert(pm)) {
        zWarning("⚠️ Nem sikerült regisztrálni az új gyökérelemet (barcode ütközés?)");
        return;
    }

    populate();
}

void ProductTreeManager::addChildProduct() {
    auto index = _view->currentIndex();
    if (!index.isValid()) return;

    QString parentIdStr = _model->itemFromIndex(index.sibling(index.row(), 2))->text();
    QUuid parentId(parentIdStr);

    const ProductMaster *parent =
        ProductRegistry::instance().findById(parentId);

    if (!parent) {
        zWarning("⚠️ addChildProduct: parent not found, aborting");
        return;
    }

    QString base = parent->barcode.isEmpty()
                       ? BarcodeGenerator::slugFromName(parent->name)
                       : parent->barcode;


    QString parentName = parent->name;

    ProductMaster pm;
    pm.id = QUuid::createUuid();
    pm.parentId = parentId;
    pm.name = parentName + " - új";
    pm.barcode = BarcodeGenerator::ensureUnique(base);

    if (!ProductRegistry::instance().insert(pm)) {
        zWarning("⚠️ Nem sikerült regisztrálni az új gyermeket (barcode ütközés?)");
        return;
    }

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

    bool ok = ProductRegistry::instance().remove(id);
    if (!ok) {
        // EventLogger-ba figyelmeztetés (makró)
        zEventWARN(QString("Törlés sikertelen: termék id=%1").arg(id.toString()));

        // Konzol/log üzenet a fejlesztőknek
        zWarning() << "Failed to remove product with id" << id;

        return;
    }

    populate();
}


void ProductTreeManager::onItemChanged(QStandardItem* item) {
    static bool _rollbackGuard = false;
    if (_rollbackGuard) return;

    auto index = item->index();
    QString idStr = _model->itemFromIndex(index.sibling(index.row(), 2))->text();
    QUuid id(idStr);

    if (const ProductMaster* pm = ProductRegistry::instance().findById(id)) {
        // készítünk egy munka-másolatot
        ProductMaster updated = *pm;

        if (item->column() == 0) { // name
            updated.name = item->text();
            zEvent(QString("✏️ Product renamed: %1").arg(item->text()));
        } else if (item->column() == 1) { // barcode
            QString newCode = item->text();

            // Validálás: nem lehet üres
            if (newCode.isEmpty()) {
                zEvent("⚠️ Barcode nem lehet üres");
                item->setText(pm->barcode); // visszaállítjuk
                return;
            }
            
            QString err;
            if (!BarcodeValidator::checkAndRegister_UI(newCode,
                                                      "Product",
                                                      pm->id,
                                                      pm->name,
                                                      err))
            {
                zEventWARN(err);
                zWarning(QString("ProductTreeManager: barcode update failed → rollback: %1 → %2")
                             .arg(newCode, pm->barcode));

                _rollbackGuard = true;
                item->setText(pm->barcode);
                _rollbackGuard = false;

                return;
            }
            
            updated.barcode = newCode; // Validator már regisztrálta
            zEvent(QString("✏️ Product barcode updated: %1").arg(newCode));
        }

        // audit-barát update → perzisztál is
        bool ok = ProductRegistry::instance().update(updated);
        if (!ok) {
            if (item->column() == 0) {
                // name rollback
                zEventWARN(QString("Product update failed: id=%1, field=name").arg(id.toString()));
                zWarning(QString("ProductTreeManager: name update failed → rollback: %1 → %2")
                             .arg(item->text(), pm->name));

                _rollbackGuard = true;
                item->setText(pm->name);
                _rollbackGuard = false;
            }
            else if (item->column() == 1) {
                // barcode rollback
                zEventWARN(QString("Product update failed: id=%1, field=barcode").arg(id.toString()));
                zWarning(QString("ProductTreeManager: barcode update failed at workflow → rollback: %1 → %2")
                             .arg(item->text(), pm->barcode));

                _rollbackGuard = true;
                item->setText(pm->barcode);
                _rollbackGuard = false;
            }

            return;
        }
         else {
            // Sikeres update esemény (audit)
            zEvent(QString("✏️ Product updated: id=%1").arg(id.toString()));
        }

    }
}


void ProductTreeManager::onProductMoved(const QUuid& id, const QUuid& newParentId) {
    Q_UNUSED(id)
    Q_UNUSED(newParentId)
    populate(); // újraépíti a fát, stílusok helyreállnak
}


void ProductTreeManager::onCurrentChanged(const QModelIndex& current,
                                          const QModelIndex& /*previous*/)
{
    if (!current.isValid()) return;

    QString idStr = _model->itemFromIndex(current.sibling(current.row(), 2))->text();
    QUuid id(idStr);

    QString name = _model->itemFromIndex(current.sibling(current.row(), 0))->text();
    QString barcode = _model->itemFromIndex(current.sibling(current.row(), 1))->text();

    emit currentProductChanged(id, name, barcode);
}

QUuid ProductTreeManager::currentProductId() const {
    auto index = _view->currentIndex();
    if (!index.isValid()) return QUuid();

    QString idStr = _model->itemFromIndex(index.sibling(index.row(), 2))->text();
    return QUuid(idStr);
}

QString ProductTreeManager::currentProductName() const {
    auto index = _view->currentIndex();
    if (!index.isValid()) return QString();

    return _model->itemFromIndex(index.sibling(index.row(), 0))->text();
}

QString ProductTreeManager::currentProductBarcode() const {
    auto index = _view->currentIndex();
    if (!index.isValid()) return QString();

    return _model->itemFromIndex(index.sibling(index.row(), 1))->text();
}
