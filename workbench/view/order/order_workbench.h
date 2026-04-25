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

    void restoreUiState();
    void saveUiState();

protected:
    void showEvent(QShowEvent* event) override;

private:
    // Layout
    QVBoxLayout* _layout = nullptr;
    QSplitter* _splitter = nullptr;

    // Panels
    OrderHeaderPanel* _headerPanel = nullptr;
    OrderItemTable* _itemTable = nullptr;

    // Toolbar
    QToolBar* _toolbar = nullptr;

    // State restore flags
    bool _restoredOnce = false;
    bool _isFullyShown = false;
    bool _canRestore = true;

    // Builders
    void buildToolbar();
    void buildPanels();
    void tryRestore();
};
