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

    // HEADER TOOLBAR ITT ÉPÜL – a headerek listája fölé kerül
    auto* headerToolbar = _presenter->buildHeaderToolbar(_listPanel);
    headerToolbar->setObjectName("order_header_toolbar");
    if (auto* listLayout = qobject_cast<QVBoxLayout*>(_listPanel->layout())) {
        listLayout->insertWidget(1, headerToolbar);
    }

    _presenter->refreshHeaderOverlay();

    // Presenter által épített item toolbar
    _itemToolbar = _presenter->buildItemToolbar(_rightSplitter);
    _itemToolbar->setObjectName("order_item_toolbar");

    if (auto* itemContainer = _itemTable->parentWidget()) {
        if (auto* itemLayout = qobject_cast<QVBoxLayout*>(itemContainer->layout())) {
            itemLayout->insertWidget(0, _itemToolbar);
        }
    }

    // listPanel a splitterbe
    _splitter->addWidget(_listPanel);
    _splitter->addWidget(_rightSplitter);

    // itemToolbar beillesztése a buildPanels által létrehozott itemContainer layout elejére
    // if (auto* itemContainer = _itemTable->parentWidget()) {
    //     if (auto* itemLayout = qobject_cast<QVBoxLayout*>(itemContainer->layout())) {
    //         itemLayout->insertWidget(0, _itemToolbar);
    //     }
    // }
}

// void OrderWorkbench::buildToolbar()
// {
//     _toolbar = new QToolBar("Order Actions", this);
//     _toolbar->setObjectName("order_toolbar");

//     _toolbar->addAction("🆕 Új rendelés");
//     _toolbar->addAction("💾 Mentés");
//     _toolbar->addAction("📂 Betöltés");
//     _toolbar->addAction("🗑️ Törlés");

// }

void OrderWorkbench::buildPanels()
{
    _headerPanel = new OrderHeaderPanel(_rightSplitter);
    _headerPanel->setObjectName("order_header_panel");


    // Placeholder a header panel helyére, ha nincs kiválasztott order
    _headerPlaceholder = new QWidget(_rightSplitter);
    auto* phLayout = new QVBoxLayout(_headerPlaceholder);
    phLayout->setAlignment(Qt::AlignCenter);

    auto* phLabel = new QLabel(
        "📭 Nincs kiválasztott rendelés\n\n"
        "A bal oldali listában válassz egy rendelést,\n"
        "vagy kattints a „+ Új” gombra egy új rendelés létrehozásához.",
        _headerPlaceholder
        );
    phLabel->setAlignment(Qt::AlignCenter);
    phLabel->setStyleSheet("font-size: 16px; color: #666;");

    phLayout->addWidget(phLabel);

    // Alapértelmezetten a placeholder látszik
    _headerPlaceholder->show();
    _headerPanel->hide();

    _itemTable = new OrderItemTable(_rightSplitter);
    _itemTable->setObjectName("order_item_table");

    _rightSplitter->addWidget(_headerPanel);

    auto* itemContainer = new QWidget(_rightSplitter);

    // Placeholder az item tábla helyére, ha nincs kiválasztott order
    _itemPlaceholder = new QWidget(_rightSplitter);
    auto* iphLayout = new QVBoxLayout(_itemPlaceholder);
    iphLayout->setAlignment(Qt::AlignCenter);

    auto* iphLabel = new QLabel(
        "📦 Nincsenek tételek\n\n"
        "A tételek itt fognak megjelenni, miután létrehozol vagy kiválasztasz egy rendelést.",
        _itemPlaceholder
        );
    iphLabel->setAlignment(Qt::AlignCenter);
    iphLabel->setStyleSheet("font-size: 16px; color: #666;");

    iphLayout->addWidget(iphLabel);

    // Alapértelmezetten a placeholder látszik
    _itemPlaceholder->show();
    _itemTable->hide();

    auto* itemLayout = new QVBoxLayout(itemContainer);
    itemLayout->setContentsMargins(0,0,0,0);
    // itemToolbar a ctor-ban kerül beillesztésre (Presenter építi)
    itemLayout->addWidget(_itemTable);
    itemContainer->setLayout(itemLayout);
    _rightSplitter->addWidget(itemContainer);

    // itemToolbar a Presenter által épül, itt illesztjük be a tábla fölé
    // if (_itemToolbar)
    //     itemLayout->insertWidget(0, _itemToolbar);

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


void OrderWorkbench::showHeaderPlaceholder(bool show)
{
    if (!_headerPlaceholder || !_headerPanel)
        return;

    if (show) {
        _headerPanel->hide();
        _headerPlaceholder->show();
    } else {
        _headerPlaceholder->hide();
        _headerPanel->show();
    }
}

void OrderWorkbench::showItemPlaceholder(bool show)
{
    if (!_itemPlaceholder || !_itemTable)
        return;

    if (show) {
        _itemTable->hide();
        _itemPlaceholder->show();
    } else {
        _itemPlaceholder->hide();
        _itemTable->show();
    }
}
