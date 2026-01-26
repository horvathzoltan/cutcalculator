#include "calculation/view/calculation_mode_detail_view.h"
#include <QHeaderView>
#include <QTableWidgetItem>

CalculationModeDetailView::CalculationModeDetailView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    _table = new QTableWidget(this);
    setup_table();
    layout->addWidget(_table);
    setLayout(layout);

    //updateOverlay(0, 0);
}

void CalculationModeDetailView::setup_table() {
    _table->setColumnCount(3);
    _table->setHorizontalHeaderLabels({"Material", "Formula", "Type"});
    _table->horizontalHeader()->setStretchLastSection(false);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    _table->setColumnWidth(0, 360);
    _table->setColumnWidth(1, 200);
    _table->setColumnWidth(2, 80);
}

void CalculationModeDetailView::set_details(const QVector<DetailRow>& rows) {
    _table->clearContents();
    _table->setRowCount(rows.size());
    for (int i=0; i<rows.size(); ++i) {
        const auto& r = rows[i];
        auto* matItem = new QTableWidgetItem(QString("%1 (%2)").arg(r.materialName, r.materialBarcode));
        auto* formItem = new QTableWidgetItem(r.formula);
        auto* typeItem = new QTableWidgetItem(r.isCutting ? "⚙️" : "📦");

        matItem->setData(Qt::UserRole, r.id); // store detailId for actions
        _table->setItem(i,0,matItem);
        _table->setItem(i,1,formItem);
        _table->setItem(i,2,typeItem);

        apply_row_visuals(i, r);

        QString tip;
        if (!r.materialValid)
            tip = "Ismeretlen anyag";
        else if (!r.formulaValid)
            tip = "Érvénytelen formula";
        else if (!r.matrixComplete)
            tip = "Hiányzó detail a mátrixban";

        if (!tip.isEmpty()) {
            for (int col = 0; col < _table->columnCount(); ++col)
                _table->item(i, col)->setToolTip(tip);
        }

    }


}

void CalculationModeDetailView::apply_row_visuals(int row, const DetailRow& d)
{
    QColor bg;
    QString tip;

    if (!d.materialValid) {
        bg = QColor("#ffcccc");
        tip = "Material not found in MaterialMaster!";
    }
    else if (!d.formulaValid) {
        bg = QColor("#ffcccc");
        tip = "Invalid formula!";
    }
    else if (!d.matrixComplete) {
        bg = QColor("#fff4c2");
        tip = "Matrix incomplete for this calculation mode.";
    }
    else {
        bg = Qt::NoBrush;
    }

    for (int col = 0; col < _table->columnCount(); ++col) {
        if (auto* item = _table->item(row, col)) {
            item->setBackground(bg);
            if (!tip.isEmpty())
                item->setToolTip(item->toolTip() + "\n" + tip);
        }
    }
}



void CalculationModeDetailView::set_current_calculation(const QUuid& calcId, const QString&) {
    _current_calcId = calcId;
    //emit request_add_detail(_current_calcId);
}

// void CalculationModeDetailView::updateOverlay(int repoCount, int visibleRows)
// {
//     if (!_statusWidget) return;
//     _statusWidget->updateOverlayState2(repoCount, visibleRows);
// }

void CalculationModeDetailView::reset()
{
    _table->clearContents();
    _table->setRowCount(0);
    _current_calcId = QUuid();
}

int CalculationModeDetailView::rowCount() const
{
    return _table ? _table->rowCount() : 0;
}
