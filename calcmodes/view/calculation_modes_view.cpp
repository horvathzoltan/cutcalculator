#include "calcmodes/view/calculation_modes_view.h"
#include <QHeaderView>
#include <QTableWidgetItem>

CalculationModesView::CalculationModesView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    _table = new QTableWidget(this);
    setup_table();
    layout->addWidget(_table);
    setLayout(layout);

    connect(_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        on_selection_changed();
    });

    //updateOverlayState();
}

void CalculationModesView::setup_table() {
    _table->setColumnCount(2);
    _table->setHorizontalHeaderLabels({"Mode", "Details"});
    _table->horizontalHeader()->setStretchLastSection(true);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    _table->setColumnWidth(0, 280);
    _table->setColumnWidth(1, 120);
}

void CalculationModesView::set_modes(const QVector<ModeRow>& rows) {
    _table->clearContents();
    _table->setRowCount(rows.size());
    for (int i=0; i<rows.size(); ++i) {
        const auto& r = rows[i];
        auto* modeItem = new QTableWidgetItem(r.modeName);
        auto* cntItem  = new QTableWidgetItem(QString::number(r.detailsCount));

        modeItem->setData(Qt::UserRole, r.id);
        _table->setItem(i,0,modeItem);
        _table->setItem(i,1,cntItem);
    }

    updateOverlayState(); // <<< itt
}

void CalculationModesView::on_selection_changed() {
    auto ranges = _table->selectedRanges();
    if (ranges.isEmpty()) {
        emit selection_changed(std::nullopt);
        return;
    }
    int row = ranges.first().topRow();
    auto* item = _table->item(row,0);
    if (!item) { emit selection_changed(std::nullopt); return; }
    emit selection_changed(item->data(Qt::UserRole).toUuid());
}

void CalculationModesView::set_current_product(const QUuid& productId,
                                               const QString& productName,
                                               const QString& productBarcode)
{
    _current_productId = productId;
    //emit current_product_changed(productId, productName, productBarcode);
}


std::optional<QUuid> CalculationModesView::currentModeId() const {
    auto ranges = _table->selectedRanges();
    if (ranges.isEmpty()) return std::nullopt;
    int row = ranges.first().topRow();
    auto* item = _table->item(row,0);
    if (!item) return std::nullopt;
    return item->data(Qt::UserRole).toUuid();
}

void CalculationModesView::updateOverlayState()
{
    if (!_statusWidget)
        return;

    const int totalCount = _table->rowCount();
    int visibleCount = 0;

    for (int r = 0; r < totalCount; ++r) {
        if (!_table->isRowHidden(r))
            visibleCount++;
    }

    ModeListState state;

    if (totalCount == 0)
        state = ModeListState::NoData;
    else if (visibleCount == 0)
        state = ModeListState::NoFilteredData;
    else
        state = ModeListState::Ok;

    // Base emoji mindig dokumentum
    _statusWidget->setBaseEmoji("📄");

    // Overlay állapot beállítása
    switch (state) {
    case ModeListState::NoData:
        _statusWidget->setOverlay(OverlayIconWidget::BottomRight, "❌" );
        break;

    case ModeListState::NoFilteredData:
        _statusWidget->setOverlay(OverlayIconWidget::BottomRight, "🔍",Qt::red);
        break;

    case ModeListState::Ok:
        _statusWidget->setOverlay(OverlayIconWidget::BottomRight, "🟢");
        break;
    }
}

