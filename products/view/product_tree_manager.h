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

    /// Visszaadja az aktuálisan kijelölt product UUID-ját
    QUuid currentProductId() const;

    /// Visszaadja az aktuálisan kijelölt product nevét
    QString currentProductName() const;

    /// Visszaadja az aktuálisan kijelölt product barcode-ját
    QString currentProductBarcode() const;

private:
    QTreeView* _view = nullptr;
    QStandardItemModel* _model = nullptr;

    void buildSubtree(QStandardItem* parentItem, const QUuid& parentId);
    void styleItem(QStandardItem* item, bool isLeaf, bool isRoot = false);

    void onItemChanged(QStandardItem* item);

public slots:
    void addRootProduct();
    void addChildProduct();
    void renameProduct();
    void removeProduct();

private slots:
    void onProductMoved(const QUuid& id, const QUuid& newParentId);
    void onCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

signals:
    /**
     * @brief Jelzi, hogy a fa aktuális kiválasztása megváltozott.
     * * amikor a user másik productot kattint, a view‑k (pl. MaterialRequirementsView)
     * tudják, melyik producthoz kell mutatni az adatokat.
     */
    void currentProductChanged(const QUuid& id,
                               const QString& name,
                               const QString& barcode);

};
