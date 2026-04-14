#include "calculation/view/calculation_mode_detail_view.h"
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QTableWidgetItem>
#include "calculation/dialogs/formula_editor_dialog.h"
#include "colors/model/colorconstants.h"
#include "common/utils/font_utils.h"
#include <calculation/registry/need_calculation_detail_registry.h>
#include <QStyledItemDelegate>
#include <common/layout/layout_default_store.h>
#include <common/utils/geometry_helper.h>

#include "dsl/formula_analysis.h"
#include "dsl/formula_contract.h"
#include "calculation/model/need_calculation_detail.h"

class FormulaSyntaxDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        // 🔧 NINCS syntax highlight, NINCS HTML
        QStyledItemDelegate::paint(painter, option, index);

        // QString text = index.data().toString();

        // // Syntax highlight
        // text.replace("len:w-", "<span style='color:#0077cc;'>len:w-</span>");
        // text.replace("len:h-", "<span style='color:#0077cc;'>len:h-</span>");
        // text.replace("qty:fixed:", "<span style='color:#cc7700;'>qty:fixed:</span>");
        // text.replace("qty:perOrder:", "<span style='color:#9933cc;'>qty:perOrder:</span>");
        // text.replace("qty:perArea:", "<span style='color:#009977;'>qty:perArea:</span>");

        // // 🔧 ÚJ: alapértelmezett szín beállítása
        // text = QString("<span style='color:black;'>%1</span>").arg(text);

        // QStyleOptionViewItem opt(option);
        // initStyleOption(&opt, index);
        // opt.text = text;
        // opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    }

    bool helpEvent(QHelpEvent*, QAbstractItemView*, const QStyleOptionViewItem&, const QModelIndex&) override {
        return false;
    }
};

CalculationModeDetailView::CalculationModeDetailView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    _table = new QTableWidget(this);
    setup_table();
    FontUtils::applySafeMonospaceFont(this);

    //_table->setPalette(qApp->palette());

    connect(_table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        if (item->column() != 1)
            return;

        int row = item->row();
        QString formula = item->text().trimmed();

        bool empty = formula.isEmpty();

        // bool isCutting = true;
        // if (auto* typeItem = _table->item(row, 3)) {
        //     isCutting = typeItem->data(Qt::UserRole).toBool();
        // }
        bool isCutting = false;
        if (auto* typeItem = _table->item(row, 3))
        {
            isCutting = typeItem->data(Qt::UserRole).toBool();
        }

        // 1) Inline validáció
        FormulaContract contract = isCutting ? cuttingContract() : kittingContract();


        FormulaAnalysis analysis = analyzeFormula(formula, contract);
        bool formulaValid = empty || analysis.ok;

        // 2) Vizuális visszajelzés
        updateRowVisuals(row, true, formulaValid, empty);

        // 2) Tooltip a hiba okáról
        QString tip;
        if (!formulaValid) {
            if (!analysis.errors.isEmpty())
                tip = analysis.errors.join("\n");   // több hiba → több sor
            else
                tip = "Invalid formula.";
        } else if (empty) {
            tip = "Formula empty.";
        } else {
            tip.clear();
        }

        int cmax = _table->columnCount();

        if (auto* it = _table->item(row, 1)) it->setToolTip(tip);
        if (auto* it = _table->item(row, 4)) it->setToolTip(tip);


        // tooltip beállítása
        // for (int col = 0; col < _table->columnCount(); ++col) {
        //     if (auto* it = _table->item(row, col))
        //         it->setToolTip(tip);
        // }


        // 4) Undo/Redo stack
        if (_lastFormulaValue != item->text()) {
            _undoStack.push_back(_lastFormulaValue);
            _redoStack.clear();
        }
    });

    connect(_table, &QTableWidget::cellDoubleClicked,
            this, [this](int row, int column) {

                Q_UNUSED(column);

                // A sorhoz tartozó detail ID
                QUuid id = _table->item(row, 0)->data(Qt::UserRole).toUuid();

                // Ugyanaz, mint a Szerkesztés gomb
                emit request_open_formula_editor(id);
            });


    connect(this, &CalculationModeDetailView::request_open_formula_editor,
            this, [this](const QUuid& id) {

                // 1) aktuális formula kiolvasása
                QString current;
                for (int row = 0; row < _table->rowCount(); ++row) {
                    if (_table->item(row, 0)->data(Qt::UserRole).toUuid() == id) {
                        current = _table->item(row, 1)->text();
                        break;
                    }
                }

                // 2) dialógus megnyitása
                NeedCalculationDetail::DetailKind kind = NeedCalculationDetail::DetailKind::Cutting;
                for (int r = 0; r < _table->rowCount(); ++r) {
                    if (_table->item(r, 0)->data(Qt::UserRole).toUuid() == id) {
                        int kindVal = _table->item(r, 3)->data(Qt::UserRole).toInt();
                        kind = static_cast<NeedCalculationDetail::DetailKind>(kindVal);

                        break;
                    }
                }
                FormulaEditorDialog dlg(current, kind, this);


                if (dlg.exec() != QDialog::Accepted)
                    return;

                // 3) új formula visszaküldése
                emit request_update_formula(id, dlg.formula());
            });

      _table->installEventFilter(this);
    _table->setItemDelegateForColumn(1, new FormulaSyntaxDelegate(_table));


    connect(_table, &QTableWidget::itemPressed, this, [this](QTableWidgetItem* item) {
        if (item->column() == 1) {
            _lastFormulaValue = item->text();
        }
    });

    layout->addWidget(_table);
    setLayout(layout);
}

void CalculationModeDetailView::setup_table() {
    _table->setColumnCount(5);
    _table->setHorizontalHeaderLabels({"Material", "Formula", "MatMode", "FormType", "State"});

    _table->horizontalHeader()->setStretchLastSection(false);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    // _table->setColumnWidth(0, 360);
    // _table->setColumnWidth(1, 220);
    // _table->setColumnWidth(2, 30);
    // _table->setColumnWidth(3, 30);
    // _table->setColumnWidth(4, 30);
}

void CalculationModeDetailView::set_details(const QVector<DetailRow>& rows) {
    _table->clearContents();
    _table->setRowCount(rows.size());

    for (int i=0; i<rows.size(); ++i) {
        renderRow(i, rows[i]);
    }
}


void CalculationModeDetailView::updateRowVisuals(
    int row,
    bool materialValid,
    bool formulaValid,
    bool empty)
{
    QString icon;
    QString tip;

    if (!materialValid) {
        icon = "❌";
        tip = "Material not found in MaterialMaster.";
    }
    else if (!formulaValid) {
        icon = "❗";
        tip = "Invalid formula.";
    }
    else if (empty) {
        icon = "🟡";
        tip = "Formula empty.";
    }
    else {
        icon = "🟢";
        tip = "";
    }

    if (auto* stateItem = _table->item(row, 4))
    {
        stateItem->setText(icon);
        stateItem->setToolTip(tip);
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

void CalculationModeDetailView::scrollToRow(int row)
{
    if (row < 0 || row >= _table->rowCount())
        return;

    _table->scrollToItem(_table->item(row, 1),
                         QAbstractItemView::PositionAtCenter);
    _table->setCurrentCell(row, 0);
}

bool CalculationModeDetailView::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == _table && ev->type() == QEvent::KeyPress) {
        auto* key = static_cast<QKeyEvent*>(ev);

        // Ctrl+Z → Undo
        if (key->modifiers() & Qt::ControlModifier && key->key() == Qt::Key_Z) {
            if (!_undoStack.isEmpty()) {
                QString prev = _undoStack.takeLast();
                _redoStack.push_back(_table->currentItem()->text());
                {
                    QSignalBlocker b(_table);
                    _table->currentItem()->setText(prev);
                }

            }
            return true;
        }

        // Ctrl+Y → Redo
        if (key->modifiers() & Qt::ControlModifier && key->key() == Qt::Key_Y) {
            if (!_redoStack.isEmpty()) {
                QString next = _redoStack.takeLast();
                _undoStack.push_back(_table->currentItem()->text());
                {
                    QSignalBlocker b(_table);
                    _table->currentItem()->setText(next);
                }

            }
            return true;
        }

        // Enter → következő sor
        if (key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) {
            int row = _table->currentRow();
            int next = row + 1;
            if (next < _table->rowCount()) {
                _table->setCurrentCell(next, 1);
            }
            return true;
        }

        // Shift+Enter → előző sor
        if ((key->modifiers() & Qt::ShiftModifier) &&
            (key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter)) {
            int row = _table->currentRow();
            int prev = row - 1;
            if (prev >= 0) {
                _table->setCurrentCell(prev, 1);
            }
            return true;
        }

        // Esc → visszaállítás
        if (key->key() == Qt::Key_Escape) {
            if (_table->currentItem()) {
                QSignalBlocker b(_table);
                _table->currentItem()->setText(_lastFormulaValue);
            }
            return true;
        }

    }

    return QWidget::eventFilter(obj, ev);
}

void CalculationModeDetailView::renderRow(int i, const DetailRow& r)
{
    auto* matItem = new QTableWidgetItem(QString("%1 (%2)").arg(r.materialName, r.materialBarcode));
    matItem->setData(Qt::UserRole, r.id);
    matItem->setToolTip("");

    auto* formItem = new QTableWidgetItem(r.formula);
    formItem->setData(Qt::DisplayRole, r.formula);
    formItem->setFlags(formItem->flags() | Qt::ItemIsEditable);

    QString matIcon =
        (r.matMode == CuttingMode::Length) ? "📏" :
            (r.matMode == CuttingMode::Piece)  ? "🔩" :
            (r.matMode == CuttingMode::None)   ? "🚫" : "❓";
    auto* matModeItem = new QTableWidgetItem(matIcon);
    matModeItem->setToolTip(
        (r.matMode == CuttingMode::Length) ? "Length" :
            (r.matMode == CuttingMode::Piece)  ? "Piece"  :
            (r.matMode == CuttingMode::None)   ? "None"   : "Unknown"
        );

    bool isCutting = r.kind == NeedCalculationDetail::DetailKind::Cutting;
    QString formIcon = isCutting ? "⚙️" : "📦";
    auto* formTypeItem = new QTableWidgetItem(formIcon);
    formTypeItem->setData(Qt::UserRole, static_cast<int>(r.kind));

    formTypeItem->setToolTip(isCutting ? "Cutting" : "Kitting");

    auto* stateItem = new QTableWidgetItem();

    _table->setItem(i,0,matItem);
    _table->setItem(i,1,formItem);
    _table->setItem(i,2,matModeItem);
    _table->setItem(i,3,formTypeItem);
    _table->setItem(i,4,stateItem);

    updateRowVisuals(i, r.materialValid, r.formulaValid, r.formula.trimmed().isEmpty());

    QString tip;
    if (!r.materialValid)
        tip = "Material missing";
    else if (!r.formulaValid)
        tip = "Invalid formula";
    else if (r.formula.trimmed().isEmpty())
        tip = "Formula empty";

    if (!tip.isEmpty()) {
        if (auto* it = _table->item(i, 1)) it->setToolTip(tip);
    }
}

void CalculationModeDetailView::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    // header restore handled by BOMWorkbench; no fallback restore here

    // const QString pct =
    //     LayoutDefaultStore::instance().calcDetailHeaderPercent();

    // if (!pct.isEmpty()) {
    //     GeometryHelper::restoreHeaderState(_table->horizontalHeader(), pct);
    // }
}

void CalculationModeDetailView::restoreHeader()
{
    const QString pct =
        LayoutDefaultStore::instance().calcDetailHeaderPercent();

    zInfo(QString("🟩 [DetailView] Restoring header pct = '%1'").arg(pct));

    if (!pct.isEmpty()) {
        GeometryHelper::restoreHeaderState(_table->horizontalHeader(), pct);

        // restore után azonnal kiolvassuk a tényleges szélességeket
        QList<int> widths;
        for (int i = 0; i < _table->columnCount(); ++i)
            widths << _table->columnWidth(i);

        QStringList ws;
        for (int w : widths)
            ws << QString::number(w);

        zInfo(QString("🟩 [DetailView] After restore widths = %1").arg(ws.join(",")));

    }
}


// void CalculationModeDetailView::closeEvent(QCloseEvent* e)
// {
//     const QString pct =
//         GeometryHelper::saveHeaderState(_table->horizontalHeader());

//     zInfo(QString("🟦 [DetailView] Saving header pct = '%1'").arg(pct));

//     LayoutDefaultStore::instance().setCalcDetailHeaderPercent(pct);
//     LayoutDefaultStore::instance().flush();

//     QWidget::closeEvent(e);
// }

