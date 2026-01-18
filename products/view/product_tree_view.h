// products/view/product_tree_view.h
#pragma once
#include <QTreeView>
#include <QDropEvent>
#include <QVBoxLayout>
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
    explicit ProductTreeView(QWidget* parent = nullptr);
    void setStatusWidget(QWidget* w);


//private:
    //QVBoxLayout* _layout = nullptr;
    //QWidget* _statusWidget = nullptr;

protected:
    void dropEvent(QDropEvent* event) override;

signals:
    void productMoved(const QUuid& id, const QUuid& newParentId);
};
