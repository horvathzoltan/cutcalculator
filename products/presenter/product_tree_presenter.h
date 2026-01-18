#pragma once
#include <QObject>
#include <QToolBar>
#include "products/view/product_tree_panel.h"
#include "products/view/product_tree_view.h"
#include "products/view/product_tree_manager.h"
#include "ui/widgets/overlay_icon_widget.h"
//#include "products/registry/product_registry.h"

class ProductTreePresenter : public QObject {
    Q_OBJECT
public:
    explicit ProductTreePresenter(ProductTreePanel* panel,
                                  ProductTreeManager* manager,
                                  QObject* parent = nullptr);

    QToolBar* buildToolbar(QWidget* parent);

private:    
    ProductTreePanel* _panel = nullptr;
    ProductTreeView* _view = nullptr; // a panelből szerezzük
    ProductTreeManager* _manager = nullptr;
    OverlayIconWidget* _status = nullptr;

    void initialOverlay();
    void connectRegistry();
    void connectTreeStats();
};
