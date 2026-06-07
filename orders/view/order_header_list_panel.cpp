#include "order_header_list_panel.h"
#include "orders/registry/order_header_registry.h"
#include <QHeaderView>
#include <QHBoxLayout>

OrderHeaderListPanel::OrderHeaderListPanel(QWidget* parent)
    : QWidget(parent)
{
    _layout = new QVBoxLayout(this);

    // --- Filter mezők ---
    _filterCustomer = new QLineEdit(this);
    _filterCustomer->setPlaceholderText("Customer…");

    _filterFrom = new QDateEdit(this);
    _filterFrom->setCalendarPopup(true);
    _filterFrom->setDisplayFormat("yyyy-MM-dd");

    _filterTo = new QDateEdit(this);
    _filterTo->setCalendarPopup(true);
    _filterTo->setDisplayFormat("yyyy-MM-dd");

    _filterStatus = new QComboBox(this);
    _filterStatus->addItems({"ALL", "NEW", "IN_PROGRESS", "DONE"});

    auto* filterLayout = new QHBoxLayout();
    filterLayout->addWidget(_filterCustomer);
    filterLayout->addWidget(_filterFrom);
    filterLayout->addWidget(_filterTo);
    filterLayout->addWidget(_filterStatus);

    _layout->addLayout(filterLayout);

    // --- Lista ---
    _table = new QTableWidget(this);
    _table->setColumnCount(4);
    _table->setHorizontalHeaderLabels({"Customer", "Order date", "Deadline", "Status"});
    _table->horizontalHeader()->setStretchLastSection(true);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);

    _layout->addWidget(_table);

    // --- Jelek ---
    connect(_filterCustomer, &QLineEdit::textChanged, this, &OrderHeaderListPanel::onFilterChanged);
    connect(_filterFrom, &QDateEdit::dateChanged, this, &OrderHeaderListPanel::onFilterChanged);
    connect(_filterTo, &QDateEdit::dateChanged, this, &OrderHeaderListPanel::onFilterChanged);
    connect(_filterStatus, &QComboBox::currentTextChanged, this, &OrderHeaderListPanel::onFilterChanged);

    connect(_table, &QTableWidget::cellClicked, this,
            [this](int row, int /*col*/) {
                auto* idItem = _table->item(row, 0);
                if (!idItem)
                    return;

                QUuid id(idItem->data(Qt::UserRole).toString());
                emit orderSelected(id);
            });

    rebuildList();
}

void OrderHeaderListPanel::onFilterChanged()
{
    rebuildList();
}

void OrderHeaderListPanel::rebuildList()
{
    _table->setRowCount(0);

    const auto all = OrderHeaderRegistry::instance().readAll();

    for (const auto& h : all) {

        if (!_filterCustomer->text().isEmpty() &&
            !h.customerName.contains(_filterCustomer->text(), Qt::CaseInsensitive))
            continue;

        if (_filterFrom->date().isValid() && h.orderDate < _filterFrom->date())
            continue;

        if (_filterTo->date().isValid() && h.orderDate > _filterTo->date())
            continue;

        if (_filterStatus->currentText() != "ALL" &&
            h.status != _filterStatus->currentText())
            continue;

        int row = _table->rowCount();
        _table->insertRow(row);

        auto* customerItem = new QTableWidgetItem(h.customerName);
        customerItem->setData(Qt::UserRole, h.id.toString());   // ID tárolása
        _table->setItem(row, 0, customerItem);

        _table->setItem(row, 1, new QTableWidgetItem(h.orderDate.toString("yyyy-MM-dd")));
        _table->setItem(row, 2, new QTableWidgetItem(h.deadline.toString("yyyy-MM-dd")));
        _table->setItem(row, 3, new QTableWidgetItem(h.status));
    }
}

std::optional<QUuid> OrderHeaderListPanel::selectedOrderId() const
{
    if (!_table)
        return std::nullopt;

    auto items = _table->selectedItems();
    if (items.isEmpty())
        return std::nullopt;

    int row = items.first()->row();
    auto* idItem = _table->item(row, 0);
    if (!idItem)
        return std::nullopt;

    QUuid id(idItem->data(Qt::UserRole).toString());
    if (id.isNull())
        return std::nullopt;

    return id;
}


void OrderHeaderListPanel::selectById(const QUuid& id)
{
    for (int row = 0; row < _table->rowCount(); ++row) {
        auto* idItem = _table->item(row, 0);
        if (!idItem)
            continue;

        if (idItem->data(Qt::UserRole).toString() == id.toString()) {
            _table->selectRow(row);
            return;
        }
    }
}

void OrderHeaderListPanel::silentSelectById(const QUuid& id)
{
    QSignalBlocker b1(_table);
    selectById(id);
}

void OrderHeaderListPanel::refresh()
{
    rebuildList();
}


int OrderHeaderListPanel::visibleRowCount() const
{
    return _table ? _table->rowCount() : 0;
}

QUuid OrderHeaderListPanel::nextOrderIdAfter(const QUuid& id) const
{
    for (int row = 0; row < _table->rowCount(); ++row) {
        auto* idItem = _table->item(row, 0);
        if (!idItem)
            continue;

        if (idItem->data(Qt::UserRole).toString() == id.toString()) {
            int nextRow = row + 1;
            if (nextRow < _table->rowCount()) {
                auto* nextItem = _table->item(nextRow, 0);
                if (nextItem)
                    return QUuid(nextItem->data(Qt::UserRole).toString());
            }
            return QUuid();
        }
    }
    return QUuid();
}

