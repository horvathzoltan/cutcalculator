// products/view/product_tree_manager.h
#pragma once
#include <QObject>
#include <QTreeView>
#include <QStandardItemModel>

/**
 * ProductTreeManager
 *
 * Feladata: a ProductRegistry-ből QStandardItemModel-t építeni,
 * és azt egy QTreeView-ba betölteni.
 * A fa hierarchiát parentId alapján szervezi, gyökér elemekkel kezdve.
 *
 * Újdonság: vizuális különbségtétel Termékcsoport (ág) és Terméktípus (levél) között.
 */
class ProductTreeManager : public QObject {
    Q_OBJECT
public:
    explicit ProductTreeManager(QTreeView* view, QObject* parent = nullptr);

    /**
     * Újraépíti a modellt a registry aktuális adatai alapján.
     * - Gyökerek beillesztése
     * - Gyerekek rekurzív felépítése
     * - Stílus beállítása (félkövér kategória, dőlt fehér levél)
     */
    void populate();

    /**
     * Hozzáférés a modellhez (tesztekhez vagy további UI integrációhoz).
     */
    QStandardItemModel* model() const { return _model; }

private:
    QTreeView* _view = nullptr;
    QStandardItemModel* _model = nullptr;

    void buildSubtree(QStandardItem* parentItem, const QUuid& parentId);
    void styleItem(QStandardItem* item, bool isLeaf);

    void onItemChanged(QStandardItem* item);

public slots:
    void addRootProduct();
    void addChildProduct();
    void renameProduct();
    void removeProduct();
};
