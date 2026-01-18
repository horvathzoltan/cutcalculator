#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "products/view/product_tree_view.h"

class ProductTreePanel : public QWidget {
    Q_OBJECT
public:
    explicit ProductTreePanel(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        _layout = new QVBoxLayout(this);
        _layout->setContentsMargins(0,0,0,0);
        _layout->setSpacing(0);

        _tree = new ProductTreeView(this);
        _layout->addWidget(_tree);
    }

    ProductTreeView* tree() const { return _tree; }

    void setStatusWidget(QWidget* w) {
        if (_status) {
            _layout->removeWidget(_status);
            _status->setParent(nullptr);
        }
        _status = w;
        if (_status)
            _layout->insertWidget(0, _status);
    }

private:
    QVBoxLayout* _layout = nullptr;
    QWidget* _status = nullptr;
    ProductTreeView* _tree = nullptr;
};
