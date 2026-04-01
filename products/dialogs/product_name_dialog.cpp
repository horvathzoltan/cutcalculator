// products/dialogs/product_name_dialog.cpp
#include "products/dialogs/product_name_dialog.h"
#include <QCursor>
#include <QStyle>
#include <QTimer>

ProductNameDialog::ProductNameDialog(QWidget* parent,
                                     const QString& initialValue,
                                     std::function<bool(const QString&)> duplicateCheck)
    : QDialog(parent)
    , _duplicateCheck(std::move(duplicateCheck))
{
    setWindowTitle("Új terméktípus");
    setModal(true);
    setMinimumWidth(320);

    auto* layout = new QVBoxLayout(this);

    _label = new QLabel("Add meg a terméktípus nevét:", this);
    layout->addWidget(_label);

    _edit = new QLineEdit(this);
    _edit->setText(initialValue);
    _edit->selectAll();
    layout->addWidget(_edit);

    connect(_edit, &QLineEdit::textEdited,
            this, [this]() { onUserFoundDialog(); });

    _buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                    Qt::Horizontal,
                                    this);
    _ok = _buttons->button(QDialogButtonBox::Ok);
    layout->addWidget(_buttons);

    connect(_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(_edit, &QLineEdit::textChanged,
            this, &ProductNameDialog::onTextChanged);

    // Egér alá pozicionálás
    QTimer::singleShot(0, this, [this]() {
        CursorAwarePlacement::placeNearCursor(this);
    });

    // AttentionBlinker
    _blinker = new AttentionBlinker(this);
    _blinker->attach(this);
    QTimer::singleShot(1200, this, [this]() {
        if (!_attentionRemoved) {
            _blinker->start(AttentionBlinker::Light, AttentionBlinker::GrayMode);
        }
    });

    // Kezdeti validáció
    onTextChanged(_edit->text());
}

void ProductNameDialog::onTextChanged(const QString& text)
{
    const QString t = text.trimmed();

    bool ok = true;

    if (t.isEmpty()) {
        ok = false;
    } else if (_duplicateCheck && _duplicateCheck(t)) {
        ok = false;
    }

    _ok->setEnabled(ok);
}

QString ProductNameDialog::value() const
{
    return _edit->text().trimmed();
}

void ProductNameDialog::removeAttentionStyling()
{
    setStyleSheet("");
}

bool ProductNameDialog::event(QEvent* e)
{
    if (e->type() == QEvent::Enter) {
        onUserFoundDialog();
    }
    return QDialog::event(e);
}

void ProductNameDialog::mousePressEvent(QMouseEvent* event)
{
    onUserFoundDialog();
    QDialog::mousePressEvent(event);
}

void ProductNameDialog::onUserFoundDialog()
{
    if (_attentionRemoved)
        return;

    _attentionRemoved = true;
    if (_blinker)
        _blinker->stop();
    removeAttentionStyling();
}
