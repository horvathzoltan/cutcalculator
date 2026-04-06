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
        QString formula = item->text();

        bool empty = formula.trimmed().isEmpty();

        // bool isCutting = true;
        // if (auto* typeItem = _table->item(row, 2)) {
        //     QString icon = typeItem->text();
        //     isCutting = (icon == "⚙️");
        // }

        bool isCutting = true;
        if (auto* typeItem = _table->item(row, 2)) {
            isCutting = typeItem->data(Qt::UserRole).toBool();
        }



        bool formulaValid = [&]() {
            if (empty)
                return true;

            FormulaContract contract = isCutting ? cuttingContract() : kittingContract();
            FormulaAnalysis a = analyzeFormula(formula, contract);
            return a.ok;
        }();


        updateRowVisuals(row, true, formulaValid, empty);
        //QColor bg;
        // QString icon;

        // if (!formulaValid) {
        //     //bg = ColorConstants::ColorRed;
        //     icon = "❗";
        // } else if (empty) {
        //     //bg = ColorConstants::ColorYellow;
        //     icon = "🟡";
        // } else {
        //     //bg = Qt::NoBrush;
        //     icon = "🟢";
        // }

        // if (auto* typeItem = _table->item(row, 2))
        //     typeItem->setText(icon);

        // for (int col = 0; col < _table->columnCount(); ++col)
        //     if (auto* it = _table->item(row, col))
        //         it->setBackground(bg);

        if (_lastFormulaValue != item->text()) {
            _undoStack.push_back(_lastFormulaValue);
            _redoStack.clear();
        }


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
                FormulaEditorDialog dlg(current, this);
                if (dlg.exec() != QDialog::Accepted)
                    return;

                // 3) új formula visszaküldése
                emit request_update_formula(id, dlg.formula());
            });

    // connect(_table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
    //     if (item->column() == 1) {
    //         QUuid id = _table->item(item->row(), 0)->data(Qt::UserRole).toUuid();
    //         emit request_edit_formula(id, item->text());
    //     }
    // });

    // connect(_table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
    //     if (item->column() == 1) {

    //         // 🔧 ÚJ: üres formula → default érték
    //         // üres formula maradjon üres (domain: empty = valid)

    //         int row = item->row();
    //         QString formula = item->text();


    //         bool formulaValid = NeedCalculationDetailRegistry::isFormulaValid(formula);
    //         bool empty = formula.trimmed().isEmpty();

    //         QColor bg;
    //         QString icon;

    //         if (!formulaValid) {
    //             bg = ColorConstants::ColorRed;
    //             icon = "❗";
    //         }
    //         else if (empty) {
    //             bg = ColorConstants::ColorYellow;
    //             icon = "🟡";
    //         }
    //         else if (formula == "unknown") {
    //             bg = Qt::NoBrush;
    //             icon = "🟢";
    //         }
    //         else {
    //             bg = Qt::NoBrush;
    //             icon = "🟢";
    //         }

    //         auto* typeItem = _table->item(row, 2);
    //         if (typeItem)
    //             typeItem->setText(icon);

    //         for (int col = 0; col < _table->columnCount(); ++col) {
    //             if (auto* it = _table->item(row, col))
    //                 it->setBackground(bg);
    //         }
    //     }
    // });

    _table->installEventFilter(this);
    _table->setItemDelegateForColumn(1, new FormulaSyntaxDelegate(_table));


    connect(_table, &QTableWidget::itemPressed, this, [this](QTableWidgetItem* item) {
        if (item->column() == 1) {
            _lastFormulaValue = item->text();
        }
    });

    // connect(_table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
    //     if (item->column() == 1) {
    //         _undoStack.push_back(_lastFormulaValue);
    //         _redoStack.clear();
    //     }
    // });

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
        formItem->setData(Qt::DisplayRole, r.formula);
        formItem->setFlags(formItem->flags() | Qt::ItemIsEditable);

        auto* typeItem = new QTableWidgetItem(r.isCutting ? "⚙️" : "📦");
        typeItem->setData(Qt::UserRole, r.isCutting);

        matItem->setData(Qt::UserRole, r.id); // store detailId for actions
        _table->setItem(i,0,matItem);
        _table->setItem(i,1,formItem);
        _table->setItem(i,2,typeItem);

        updateRowVisuals(i, r.materialValid, r.formulaValid, r.formula.trimmed().isEmpty());

        QString tip;
        if (!r.materialValid)
            tip = "Material missing";
        else if (!r.formulaValid)
            tip = "Invalid formula";
        else if (r.formula.trimmed().isEmpty())
            tip = "Formula empty";
        /* removed: matrixComplete tooltip */

        if (!tip.isEmpty()) {
            for (int col = 0; col < _table->columnCount(); ++col)
                _table->item(i, col)->setToolTip(tip);
        }

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

    // ikon
    if (auto* typeItem = _table->item(row, 2))
        typeItem->setText(icon);

    // tooltip minden cellára
    for (int col = 0; col < _table->columnCount(); ++col) {
        if (auto* it = _table->item(row, col)) {
            if (!tip.isEmpty())
                it->setToolTip(tip);
            else
                it->setToolTip({});
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

