#include "colors/model/colorconstants.h"
#include "calcmodes/view/calculation_modes_view.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include "common/utils/font_utils.h"
#include "mode_row_delegate.h"

CalculationModesView::CalculationModesView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    _table = new QTableWidget(this);
    setup_table();
    FontUtils::applySafeMonospaceFont(this);

    layout->addWidget(_table);
    setLayout(layout);

    connect(_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        on_selection_changed();
    });

    _table->setItemDelegate(new ModeRowDelegate(_table));
    _table->setMouseTracking(true); // hover működjön

    connect(_table, &QTableWidget::itemDoubleClicked,
            this, &CalculationModesView::on_item_double_clicked);

}

void CalculationModesView::on_item_double_clicked(QTableWidgetItem* item)
{
    if (!item)
        return;

    int row = item->row();
    auto* idItem = _table->item(row, 0);
    if (!idItem)
        return;

    QUuid modeId = idItem->data(Qt::UserRole).toUuid();
    emit modeActivated(modeId);
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

    for (int i = 0; i < rows.size(); ++i) {
        const auto& r = rows[i];

        // --- Státusz ikon ---
        QString icon;
        if (r.hasMissingDetails) {
            icon = "🔴 ";
        }
        else if (r.detailsCount == 0) {
            icon = "🟡 ";
        }
        else {
            icon = "🟢 "; // all good
        }

        QString label = icon + r.modeName;

        auto* modeItem = new QTableWidgetItem(label);
        auto* cntItem  = new QTableWidgetItem(QString::number(r.detailsCount));

        QString tip;
        if (r.hasMissingDetails) {
            tip = "Missing detail(s)";
        }
        else if (r.detailsCount == 0) {
            tip = "Unknown formula(s)";
        }
        else {
            tip = "All details OK";
        }

        if (!tip.isEmpty())
            modeItem->setToolTip(tip);


        if (r.hasMissingDetails) {
            // modeItem->setBackground(ColorConstants::ColorRed);
            // cntItem->setBackground(ColorConstants::ColorRed);
        }
        else if (r.detailsCount == 0) {
            // modeItem->setBackground(ColorConstants::ColorYellow);
            // cntItem->setBackground(ColorConstants::ColorYellow);
        }

        /* removed: matrix-incomplete background */
        else {
            // modeItem->setBackground(Qt::NoBrush);
            // cntItem->setBackground(Qt::NoBrush);
        }


        modeItem->setData(Qt::UserRole, r.id);
        _table->setItem(i, 0, modeItem);
        _table->setItem(i, 1, cntItem);
    }
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
    emit current_product_changed(productId, productName, productBarcode);
}


std::optional<QUuid> CalculationModesView::currentModeId() const {
    auto ranges = _table->selectedRanges();
    if (ranges.isEmpty()) return std::nullopt;
    int row = ranges.first().topRow();
    auto* item = _table->item(row,0);
    if (!item) return std::nullopt;
    return item->data(Qt::UserRole).toUuid();
}

int CalculationModesView::rowCount() const
{
    return _table ? _table->rowCount() : 0;
}



