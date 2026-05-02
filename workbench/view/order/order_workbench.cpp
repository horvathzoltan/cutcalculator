#include "order_workbench.h"
#include <common/ui_state/workbench_state_manager.h>

OrderWorkbench::OrderWorkbench(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("order_workbench");

    // ⭐ REGISTER WORKBENCH
    WorkbenchStateManager::instance().registerWorkbench("order_workbench", this);

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    buildToolbar();

    const auto& actions = _toolbar->actions();

    auto newAction    = actions.at(0);   // 🆕 Új rendelés
    auto saveAction   = actions.at(1);   // 💾 Mentés
    auto loadAction   = actions.at(2);   // 📂 Betöltés
    auto deleteAction = actions.at(3);   // 🗑️ Törlés

    // top toolbar connect-ek később kerülnek be – presenter még nem létezik

    // _listPanel még nem létezik – connect később kerül be

    _layout->addWidget(_toolbar);

    // Splitter
    _splitter = new QSplitter(Qt::Horizontal, this);
    _splitter->setObjectName("order_main_splitter");
    _layout->addWidget(_splitter);

    _listPanel = new OrderHeaderListPanel(_splitter);
    _listPanel->setObjectName("order_header_list_panel");

    connect(_listPanel, &OrderHeaderListPanel::orderSelected,
            this, [this](const QUuid& id) {
                _presenter->loadOrder(id);
            });

    _rightSplitter = new QSplitter(Qt::Vertical, _splitter);
    _rightSplitter->setObjectName("order_right_splitter");

    // buildPanels csak a view-ket hozza létre (headerPanel + itemTable)
    buildPanels();

    // Manager és Presenter most jön létre
    _orderManager = new OrderManager(this);

    _presenter = new OrderWorkbenchPresenter(
        _headerPanel,
        _itemTable,
        _orderManager,
        _listPanel,
        this
        );

    connect(newAction, &QAction::triggered, this, [this]() {
        _presenter->newOrder();
    });

    connect(saveAction, &QAction::triggered, this, [this]() {
        _presenter->saveOrder();
    });

    connect(loadAction, &QAction::triggered, this, [this]() {
        QUuid id = _listPanel->selectedOrderId();
        if (!id.isNull())
            _presenter->loadOrder(id);
    });

    connect(deleteAction, &QAction::triggered, this, [this]() {
        _presenter->deleteOrder();
    });

    // Presenter által épített item toolbar
    _itemToolbar = _presenter->buildItemToolbar(_rightSplitter);
    _itemToolbar->setObjectName("order_item_toolbar");

    // listPanel a splitterbe
    _splitter->addWidget(_listPanel);
    _splitter->addWidget(_rightSplitter);

    // itemToolbar beillesztése a buildPanels által létrehozott itemContainer layout elejére
    if (auto* itemContainer = _itemTable->parentWidget()) {
        if (auto* itemLayout = qobject_cast<QVBoxLayout*>(itemContainer->layout())) {
            itemLayout->insertWidget(0, _itemToolbar);
        }
    }
}

void OrderWorkbench::buildToolbar()
{
    _toolbar = new QToolBar("Order Actions", this);
    _toolbar->setObjectName("order_toolbar");

    _toolbar->addAction("🆕 Új rendelés");
    _toolbar->addAction("💾 Mentés");
    _toolbar->addAction("📂 Betöltés");
    _toolbar->addAction("🗑️ Törlés");

}

void OrderWorkbench::buildPanels()
{
    _headerPanel = new OrderHeaderPanel(_rightSplitter);
    _headerPanel->setObjectName("order_header_panel");

    _itemTable = new OrderItemTable(_rightSplitter);
    _itemTable->setObjectName("order_item_table");

    _rightSplitter->addWidget(_headerPanel);

    auto* itemContainer = new QWidget(_rightSplitter);
    auto* itemLayout = new QVBoxLayout(itemContainer);
    itemLayout->setContentsMargins(0,0,0,0);
    // itemToolbar a ctor-ban kerül beillesztésre (Presenter építi)

    itemLayout->addWidget(_itemTable);

    itemContainer->setLayout(itemLayout);

    _rightSplitter->addWidget(itemContainer);

}

QVariantMap OrderWorkbench::saveCustomState() const
{
    QVariantMap m;

    if (_listPanel) {
        QUuid id = _listPanel->selectedOrderId();
        if (!id.isNull())
            m.insert("selectedOrderId", id.toString());
    }

    return m;
}

void OrderWorkbench::restoreCustomState(const QVariantMap& state)
{
    if (!_listPanel)
        return;

    QSignalBlocker b1(_listPanel);

    const QString idStr = state.value("selectedOrderId").toString();
    if (idStr.isEmpty())
        return;

    QUuid id(idStr);
    if (id.isNull())
        return;

    _listPanel->silentSelectById(id);
}

