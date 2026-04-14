#include "formula_editor_dialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <dsl/formula_analysis.h>
#include <dsl/formula_contract.h>
#include <QPushButton>
#include "dsl/dsl_syntax_highlighter.h"
#include "dsl/dsl_completer.h"
#include "common/utils/geometry_helper.h"
#include "common/layout/layout_default_store.h"

FormulaEditorDialog::FormulaEditorDialog(const QString& initial, NeedCalculationDetail::DetailKind kind, QWidget* parent)
    : QDialog(parent), _kind(kind)
{
    auto* layout = new QVBoxLayout(this);

    _edit = new DslTextEdit(this);
    _edit->setPlainText(initial);
    layout->addWidget(_edit);

    _hl = new DslSyntaxHighlighter(_edit->document());

    QStringList words;
    if (_kind == NeedCalculationDetail::DetailKind::Cutting) {
        words << "len:" << "qty:" << "mat:" << "w" << "h" << "qty"
              << "fixed:" << "perArea:" << "perOrder:";
    } else {
        words << "qty:" << "mat:" << "w" << "h" << "qty"
              << "fixed:" << "perOrder:";
    }

    auto* comp = new DslCompleter(words, this);
    _edit->setCompleter(comp);

    _status = new QLabel(this);
    _status->setText("");
    layout->addWidget(_status);

    _edit->setPlaceholderText(
        (_kind == NeedCalculationDetail::DetailKind::Cutting)
            ? "pl.: len:w-10, qty:fixed:3"
            : "pl.: qty:fixed:3, qty:perOrder:5"
        );

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(_edit, &QPlainTextEdit::textChanged, this, [this]() {
        QString t = _edit->toPlainText().trimmed();

        if (t.isEmpty()) {
            // Üres mező → nincs hiba, nincs overlay, nincs gutter
            _hl->setErrors({});
            _edit->setErrorMap({});
            _edit->setInlineErrors({});
            _edit->setErrorLines({});
            _status->setText("");
            auto* buttons = findChild<QDialogButtonBox*>();
            if (buttons)
                buttons->button(QDialogButtonBox::Ok)->setEnabled(true);
            return;
        }


        FormulaContract contract = (_kind == NeedCalculationDetail::DetailKind::Cutting)
                                       ? cuttingContract()
                                       : kittingContract();

        FormulaAnalysis a = analyzeFormula(t, contract);

        bool ok = t.isEmpty() || a.ok;

        auto* buttons = findChild<QDialogButtonBox*>();
        if (buttons) {
            buttons->button(QDialogButtonBox::Ok)->setEnabled(ok);
        }

        if (!ok) {
            _edit->setStyleSheet("background:#ffcccc;");
            const QString errText = a.errors.isEmpty()
                                        ? "Invalid formula."
                                        : a.errors.join("\n");
            _edit->setToolTip(errText);
            if (_status)
                _status->setText(QString("Hibák: %1").arg(a.errors.size()));
        } else {
            _edit->setStyleSheet("");
            _edit->setToolTip("");
            if (_status)
                _status->setText(t.isEmpty() ? "" : "OK");
        }

        if (!a.errors.isEmpty()) {
            _edit->setErrorLines({0});   // minden hiba az 1. sorban
        } else {
            _edit->setErrorLines({});
        }

        _hl->setErrors(a.errors);
        _edit->setErrorMap(_hl->errorMap());
        _edit->setInlineErrors(a.errors);

    });


}

QString FormulaEditorDialog::formula() const {
    return _edit->toPlainText().trimmed();
}

void FormulaEditorDialog::closeEvent(QCloseEvent* e)
{
    const QString geom = GeometryHelper::saveDialogGeometry(this);
    LayoutDefaultStore::instance().setFormulaEditorGeometryPercent(geom);
    QDialog::closeEvent(e);
}

void FormulaEditorDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);

    const QString geom =
        LayoutDefaultStore::instance().formulaEditorGeometryPercent();

    if (!geom.isEmpty()) {
        const QSize savedScreen =
            GeometryHelper::parseScreenSize(
                LayoutDefaultStore::instance().screenSizeString());

        GeometryHelper::restoreDialogGeometry(this, geom, savedScreen);
    }
}

