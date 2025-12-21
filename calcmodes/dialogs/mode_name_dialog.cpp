#include "calcmodes/dialogs/mode_name_dialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>


ModeNameDialog::ModeNameDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    connectSignals();

    setWindowTitle(tr("Új számítási mód"));
    setModal(true);

    // Induláskor a szövegmezőre fókuszálunk
    _edit->setFocus();
}

void ModeNameDialog::setupUi()
{
    auto* layout = new QVBoxLayout(this);

    _label = new QLabel(tr("Számítási mód neve:"), this);
    _edit  = new QLineEdit(this);
    _buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this
        );

    // Kezdetben az OK gomb csak akkor engedélyezett, ha nem üres a név
    auto* okButton = _buttons->button(QDialogButtonBox::Ok);
    if (okButton) {
        okButton->setEnabled(false);
    }

    layout->addWidget(_label);
    layout->addWidget(_edit);
    layout->addWidget(_buttons);

    setLayout(layout);
}

void ModeNameDialog::connectSignals()
{
    // OK / Cancel gombok
    connect(_buttons, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(_buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    // Üres név esetén ne engedjük az OK gombot
    connect(_edit, &QLineEdit::textChanged,
            this, [this](const QString& text) {
                auto* okButton = _buttons->button(QDialogButtonBox::Ok);
                if (!okButton) return;

                okButton->setEnabled(!text.trimmed().isEmpty());
            });
}

QString ModeNameDialog::modeName() const
{
    return _edit ? _edit->text().trimmed() : QString();
}
