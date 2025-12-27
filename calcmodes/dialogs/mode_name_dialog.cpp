#include "mode_name_dialog.h"
#include "common/utils/cursor_aware_placement.h"
#include <QCursor>
#include <QStyle>
#include <QTimer>
#include <QRandomGenerator>

/* ============================================================
 * 🧩 Konstruktor
 * ============================================================ */
ModeNameDialog::ModeNameDialog(QWidget* parent,
                               const QString& initialValue,
                               std::function<bool(const QString&)> duplicateCheck)
    : QDialog(parent),
    _duplicateCheck(std::move(duplicateCheck))
{
    setWindowTitle("Új számítási mód");
    setModal(true);
    setMinimumWidth(320);

    // Layout
    auto* layout = new QVBoxLayout(this);

    _label = new QLabel("Add meg a számítási mód nevét:", this);
    //_label->setStyleSheet("font-weight: bold; color: #0078D4; font-size: 14px;");
    layout->addWidget(_label);

    _edit = new QLineEdit(this);
    _edit->setText(initialValue);
    _edit->selectAll();
    layout->addWidget(_edit);

    // 3) User gépel → rátalált
    connect(_edit, &QLineEdit::textEdited, this, &ModeNameDialog::onUserFoundDialog);

    _buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    _ok = _buttons->button(QDialogButtonBox::Ok);
    layout->addWidget(_buttons);

    connect(_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Validáció
    connect(_edit, &QLineEdit::textChanged, this, &ModeNameDialog::onTextChanged);

    // Egér alá pozicionálás
    QTimer::singleShot(0, this, [this]() {
        CursorAwarePlacement::placeNearCursor(this);
    });

    // Adaptive Delay + One-shot Attention
    _blinker = new AttentionBlinker(this);
    _blinker->attach(this);
    QTimer::singleShot(1200, this, [this]() {
        if (!_attentionRemoved) {
            _blinker->start(AttentionBlinker::Light, AttentionBlinker::GrayMode);
        }
    });
}

/* ============================================================
 * 🧩 Inline validáció
 * ============================================================ */
void ModeNameDialog::onTextChanged(const QString& text) {
    QString t = text.trimmed();

    bool ok = true;
    QString tooltip;

    if (t.isEmpty() || (_duplicateCheck && _duplicateCheck(t))) {
        _ok->setEnabled(false);
    } else {
        _ok->setEnabled(true);
    }


    _ok->setEnabled(ok);

    if (!ok) {
        _edit->setStyleSheet("");
        _edit->setToolTip(tooltip);
    } else {
        _edit->setStyleSheet("");
        _edit->setToolTip("");
    }
}

/* ============================================================
 * 🧩 Visszaadott érték
 * ============================================================ */
QString ModeNameDialog::value() const {
    return _edit->text().trimmed();
}


void ModeNameDialog::removeAttentionStyling() {
    // 2) Accent keret eltávolítása
    setStyleSheet("");
}

bool ModeNameDialog::event(QEvent* e) {
    if (e->type() == QEvent::Enter) {
        onUserFoundDialog();
    }
    return QDialog::event(e);
}

void ModeNameDialog::mousePressEvent(QMouseEvent* event) {
    onUserFoundDialog();
    QDialog::mousePressEvent(event);
}

void ModeNameDialog::onUserFoundDialog() {
    if (_attentionRemoved)
        return;

    _attentionRemoved = true;
    _blinker->stop();
    setStyleSheet("");
}

