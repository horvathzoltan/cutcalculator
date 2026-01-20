#pragma once
#include <QObject>
#include <QToolBar>
#include "products/view/product_tree_panel.h"
#include "products/view/product_tree_view.h"
#include "products/view/product_tree_manager.h"
#include "products/registry/product_registry.h"
#include "ui/helpers/repository_overlay_widget.h"

class ProductTreePresenter : public QObject {
    Q_OBJECT
public:
    explicit ProductTreePresenter(ProductTreeView* view,
                                  ProductTreeManager* manager,
                                  QObject* parent = nullptr);

    QToolBar* buildToolbar(QWidget* parent);

private:    
    ProductTreeView* _view = nullptr;
    ProductTreeManager* _manager = nullptr;
    RepositoryOverlayWidget<ProductRegistry>* _status = nullptr;

    void refreshOverlayOnly();
    void connectRegistry();
    void connectTreeStats();
};
