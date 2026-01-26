#include "formula_editor_dialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

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
}

QString FormulaEditorDialog::formula() const {
    return _edit->text().trimmed();
}
