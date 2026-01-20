// products/view/product_tree_view.h
#pragma once
#include "ui/widgets/overlay_icon_widget.h"
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
    void setStatusWidget(OverlayIconWidget* w);

protected:
    void dropEvent(QDropEvent* event) override;

signals:
    void productMoved(const QUuid& id, const QUuid& newParentId);
};
