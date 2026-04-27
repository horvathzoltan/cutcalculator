#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>

#include "order_header_panel.h"
#include "order_item_table.h"

class OrderWorkbench : public QWidget {
    Q_OBJECT
public:
    explicit OrderWorkbench(QWidget* parent = nullptr);

private:
    // Layout
    QVBoxLayout* _layout = nullptr;
    QSplitter* _splitter = nullptr;

    // Panels
    OrderHeaderPanel* _headerPanel = nullptr;
    OrderItemTable* _itemTable = nullptr;

    // Toolbar
    QToolBar* _toolbar = nullptr;

    // Builders
    void buildToolbar();
    void buildPanels();
    // protected:
    //     void hideEvent(QHideEvent* e) override;


};
