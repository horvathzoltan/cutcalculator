#include "formula_editor_dialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <dsl/formula_analysis.h>
#include <dsl/formula_contract.h>
#include <QPushButton>

FormulaEditorDialog::FormulaEditorDialog(const QString& initial, QWidget* parent)
    : QDialog(parent)
{
    auto* layout = new QVBoxLayout(this);

    _edit = new QLineEdit(initial, this);
    layout->addWidget(_edit);

    _edit->setPlaceholderText("pl.: len:w-10, qty:fixed:3, qty:perArea:1000");

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(_edit, &QLineEdit::textChanged, this, [this](const QString& text) {
        QString t = text.trimmed();

        FormulaContract contract = cuttingContract();  // vagy később: paraméterezhető
        FormulaAnalysis a = analyzeFormula(t, contract);

        bool ok = t.isEmpty() || a.ok;

        auto* buttons = findChild<QDialogButtonBox*>();
        if (buttons) {
            buttons->button(QDialogButtonBox::Ok)->setEnabled(ok);
        }

        if (!ok) {
            _edit->setStyleSheet("background:#ffcccc;");
            if (!a.errors.isEmpty())
                _edit->setToolTip(a.errors.join("\n"));
            else
                _edit->setToolTip("Invalid formula.");
        } else {
            _edit->setStyleSheet("");
            _edit->setToolTip("");
        }
    });


}

QString FormulaEditorDialog::formula() const {
    return _edit->text().trimmed();
}
