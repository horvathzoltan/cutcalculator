#include "order_workbench.h"
#include <common/ui_state/workbench_state_manager.h>
#include <QTimer>
#include <orders/repository/order_item_repository.h>

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

    _listContainer = new QWidget(_splitter);
    _listContainer->setMinimumWidth(250);

    auto* lcLayout = new QVBoxLayout(_listContainer);
    lcLayout->setContentsMargins(0,0,0,0);

    // LIST PANEL
    _listPanel = new OrderHeaderListPanel(_listContainer);
    _listPanel->setObjectName("order_header_list_panel");
    _listPanel->setMinimumWidth(250);

    // PLACEHOLDER
    _listPlaceholder = new QWidget(_listContainer);
    _listPlaceholder->setMinimumWidth(250);

    auto* lpLayout = new QVBoxLayout(_listPlaceholder);
    lpLayout->setAlignment(Qt::AlignCenter);

    auto* lpLabel = new QLabel(
        "📭 Még nincs egyetlen rendelés sem\n\n"
        "Kattints a „+ Új” gombra az első rendelés létrehozásához.",
        _listPlaceholder
        );
    lpLabel->setAlignment(Qt::AlignCenter);
    lpLabel->setStyleSheet("font-size: 16px; color: #666;");
    lpLayout->addWidget(lpLabel);

    // ALAPÉRTELMEZETT ÁLLAPOT
    _listPlaceholder->show();
    _listPanel->hide();

    lcLayout->addWidget(_listPlaceholder);
    lcLayout->addWidget(_listPanel);

    connect(_listPanel, &OrderHeaderListPanel::orderSelected,
            this, [this](const QUuid& id) {
                _presenter->loadOrder(id);
            });

    _rightSplitter = new QSplitter(Qt::Vertical, _splitter);
    _rightSplitter->setObjectName("order_right_splitter");

    // buildPanels csak a view-ket hozza létre (headerPanel + itemTable)
    buildPanels();

    _itemTable->setMinimumHeight(50);
    _itemPlaceholder->setMinimumHeight(50);

    // Ha mindkét oldalon placeholder van, a splitter restore hibás lehet → erőből beállítjuk

    if (OrderHeaderRegistry::instance().size() == 0) {
        _listPlaceholder->show();
        _listPanel->hide();
    } else {
        _listPlaceholder->hide();
        _listPanel->show();
    }

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
    auto* headerToolbar = _presenter->buildHeaderToolbar(_listContainer);
    headerToolbar->setObjectName("order_header_toolbar");
    if (auto* lcLayout = qobject_cast<QVBoxLayout*>(_listContainer->layout())) {
        lcLayout->insertWidget(0, headerToolbar);
    }

    _presenter->refreshHeaderOverlay();

    // Presenter által épített item toolbar
    _itemToolbar = _presenter->buildItemToolbar(_itemContainer);
    _itemToolbar->setObjectName("order_item_toolbar");

    if (auto* itemLayout = qobject_cast<QVBoxLayout*>(_itemContainer->layout())) {
        itemLayout->insertWidget(0, _itemToolbar);
    }

    // listPanel a splitterbe
    _splitter->addWidget(_listContainer);
    _splitter->addWidget(_rightSplitter);

    // toolbarek alapból rejtve
    _itemToolbar->hide();
    //headerToolbar->hide();


    // UI állapot frissítése domain alapján
    updateUIState();

    // bool leftEmpty  = (OrderHeaderRegistry::instance().size() == 0);

    // // Most már biztonságos a show/hide
    // if (leftEmpty) {
    //     _listPlaceholder->show();
    //     _listPanel->hide();
    // } else {
    //     _listPlaceholder->hide();
    //     _listPanel->show();
    // }

    // showItemPlaceholder(true);

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
    auto* headerContainer = new QWidget(_rightSplitter);
    auto* headerLayout = new QVBoxLayout(headerContainer);
    headerLayout->setContentsMargins(0,0,0,0);

    _headerPanel = new OrderHeaderPanel(headerContainer);
    _headerPanel->setObjectName("order_header_panel");

    // Placeholder a header panel helyére, ha nincs kiválasztott order
    _headerPlaceholder = new QWidget(headerContainer);
    auto* phLayout = new QVBoxLayout(_headerPlaceholder);
    phLayout->setAlignment(Qt::AlignCenter);


    const bool hasAnyOrder =
        OrderHeaderRegistry::instance().size() > 0;

    QString headerText;

    if (!hasAnyOrder) {
        // 🔵 NINCS EGYETLEN RENDELÉS SEM → globális üresség
        headerText =
            "📭 Nincs még egyetlen rendelés sem\n\n"
            "A jobb oldalon itt fognak megjelenni a kiválasztott rendelés adatai.\n"
            "Mivel még nincs rendelés, ezért nem tudsz semmit kiválasztani.\n"
            "Kattints a „+ Új” gombra az első rendelés létrehozásához.";
    } else {
        // 🟢 VAN RENDELÉS, DE NINCS KIVÁLASZTVA → lokális üresség
        headerText =
            "📭 Nincs kiválasztott rendelés\n\n"
            "A bal oldali listából válassz egy rendelést,\n"
            "vagy kattints a „+ Új” gombra egy új rendelés létrehozásához.";
    }


    auto* phLabel = new QLabel(headerText, _headerPlaceholder);
    phLabel->setAlignment(Qt::AlignCenter);
    phLabel->setStyleSheet("font-size: 16px; color: #666;");

    phLayout->addWidget(phLabel);


    // Alapértelmezetten a placeholder látszik
    _headerPlaceholder->show();
    _headerPanel->hide();

    headerLayout->addWidget(_headerPlaceholder);
    headerLayout->addWidget(_headerPanel);

    _itemContainer = new QWidget(_rightSplitter);

    _itemTable = new OrderItemTable(_itemContainer);
    _itemTable->setObjectName("order_item_table");

    // Placeholder az item tábla helyére, ha nincs kiválasztott order
    _itemPlaceholder = new QWidget(_itemContainer);
    auto* iphLayout = new QVBoxLayout(_itemPlaceholder);
    iphLayout->setAlignment(Qt::AlignCenter);

    QString itemText;
    if (hasAnyOrder) {
        itemText =
            "📦 Nincsenek tételek\n\n"
            "A tételek itt fognak megjelenni, miután létrehozol vagy kiválasztasz egy rendelést.";
    } else {
        itemText =
            "📦 Nincsenek tételek\n\n"
            "A tételek itt fognak megjelenni, miután létrehozol egy rendelést.";
    }

    auto* iphLabel = new QLabel(itemText, _itemPlaceholder);
    iphLabel->setAlignment(Qt::AlignCenter);
    iphLabel->setStyleSheet("font-size: 16px; color: #666;");

    iphLayout->addWidget(iphLabel);


    // Alapértelmezetten a placeholder látszik
    _itemPlaceholder->show();
    _itemTable->hide();

    auto* itemLayout = new QVBoxLayout(_itemContainer);
    itemLayout->setContentsMargins(0,0,0,0);
    // itemToolbar a ctor-ban kerül beillesztésre (Presenter építi)
    itemLayout->addWidget(_itemPlaceholder);
    itemLayout->addWidget(_itemTable);
    _itemContainer->setLayout(itemLayout);

    _rightSplitter->addWidget(headerContainer);
    _rightSplitter->addWidget(_itemContainer);


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
        if (_itemToolbar) _itemToolbar->hide();
    } else {
        _itemPlaceholder->hide();
        _itemTable->show();
        if (_itemToolbar) _itemToolbar->show();
    }
}

Q_INVOKABLE void OrderWorkbench::postRestoreFix()
{
    bool leftEmpty  = (OrderHeaderRegistry::instance().size() == 0);
    bool rightEmpty = true;

    if (!(leftEmpty && rightEmpty))
        return;

    // 1) FŐ SPLITTER fix – ez mehet 0 ms késleltetéssel
    QTimer::singleShot(0, this, [this]() {

        zInfo("postRestoreFix → fő splitter fix");

        _splitter->setSizes({300, 700});

        // 2) JOBB OLDALI SPLITTER fix – csak akkor, amikor már van height
        QTimer::singleShot(0, this, [this]() {

            int h = _rightSplitter->height();
            if (h <= 0)
                return;

            int top = h * 0.3;
            int bottom = h - top;
            _rightSplitter->setSizes({top, bottom});

            zInfo("postRestoreFix → jobb splitter fix (2‑container OK)");
        });

    });
}



// void OrderWorkbench::updateListPlaceholder()
// {
//     if (!_listPanel || !_listPlaceholder)
//         return;

//     const int total = OrderHeaderRegistry::instance().size();
//     const int visible = _listPanel->visibleRowCount();

//     if (total == 0) {
//         // nincs egyetlen header sem
//         _listPlaceholder->show();
//         _listPanel->hide();
//     }
//     else if (visible == 0) {
//         // vannak headerek, de a filter miatt nincs találat
//         if (auto* lbl = _listPlaceholder->findChild<QLabel*>()) {
//             lbl->setText(
//                 "🔍 Nincs találat\n\n"
//                 "Módosítsd a fenti szűrőket."
//                 );
//         }
//         _listPlaceholder->show();
//         _listPanel->hide();
//     }
//     else {
//         // van látható header
//         _listPlaceholder->hide();
//         _listPanel->show();
//     }
// }


void OrderWorkbench::updateUIState()
{
    bool hasHeader = OrderHeaderRegistry::instance().size() > 0;

    if (!hasHeader) {
        showHeaderPlaceholder(true);
        showItemPlaceholder(true);
        return;
    }

    showHeaderPlaceholder(false);

    QUuid id = _listPanel->selectedOrderId();

    if (id.isNull()) {
        showItemPlaceholder(true);
        return;
    }

    bool hasItem = !OrderItemRegistry::instance().findByOrderId(id).isEmpty();

    if (!hasItem) {
        showItemPlaceholder(true);
    } else {
        showItemPlaceholder(false);
    }
}

