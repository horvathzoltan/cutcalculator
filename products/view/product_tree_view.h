// products/view/product_tree_view.h
#pragma once
#include <QTreeView>
#include <QDropEvent>
//#include "products/registry/product_registry.h"

/**
 * ProductTreeView
 *
 * Drag & drop átszervezés támogatása.
 * Drop után frissíti a parentId-t a ProductRegistry-ben.
 */
class ProductTreeView : public QTreeView {
    Q_OBJECT
public:
    using QTreeView::QTreeView;

protected:
    void dropEvent(QDropEvent* event) override;
};
