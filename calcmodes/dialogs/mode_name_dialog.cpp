#include "mode_name_dialog.h"
#include <QCursor>
#include <QStyle>
#include <QTimer>

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

    // Accent keret
    applyAccentFrame();

    // Layout
    auto* layout = new QVBoxLayout(this);

    _label = new QLabel("Add meg a számítási mód nevét:", this);
    _label->setStyleSheet("font-weight: bold; color: #0078D4; font-size: 14px;");
    layout->addWidget(_label);

    _edit = new QLineEdit(this);
    _edit->setText(initialValue);
    _edit->selectAll();
    layout->addWidget(_edit);

    _buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                    Qt::Horizontal, this);
    _ok = _buttons->button(QDialogButtonBox::Ok);
    layout->addWidget(_buttons);

    connect(_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(_edit, &QLineEdit::textChanged, this, &ModeNameDialog::onTextChanged);

    // Első validáció
    onTextChanged(_edit->text());

    // Egér alá pozicionálás
    QTimer::singleShot(0, this, &ModeNameDialog::positionNearCursor);
}

/* ============================================================
 * 🧩 Accent keret
 * ============================================================ */
void ModeNameDialog::applyAccentFrame() {
    setStyleSheet(R"(
        QDialog {
            border: 2px solid #0078D4;
            border-radius: 8px;
            background: #ffffff;
        }
    )");
}

/* ============================================================
 * 🖱️ Egér alá pozicionálás
 * ============================================================ */
void ModeNameDialog::positionNearCursor() {
    QPoint p = QCursor::pos() + QPoint(20, 20);

    // Multi-monitor safe
    QRect screenRect = QGuiApplication::screenAt(p)->geometry();
    QRect dlgRect = QRect(p, sizeHint());

    if (!screenRect.contains(dlgRect)) {
        p.setX(std::min(p.x(), screenRect.right() - width() - 10));
        p.setY(std::min(p.y(), screenRect.bottom() - height() - 10));
    }

    move(p);
}

/* ============================================================
 * 🧩 Inline validáció
 * ============================================================ */
void ModeNameDialog::onTextChanged(const QString& text) {
    QString t = text.trimmed();

    bool ok = true;
    QString tooltip;

    if (t.isEmpty()) {
        ok = false;
        tooltip = "A név nem lehet üres.";
    } else if (_duplicateCheck && _duplicateCheck(t)) {
        ok = false;
        tooltip = "Már létezik ilyen nevű számítási mód.";
    }

    _ok->setEnabled(ok);

    if (!ok) {
        _edit->setStyleSheet("border: 2px solid red;");
        _edit->setToolTip(tooltip);
    } else {
        _edit->setStyleSheet("");
        _edit->setToolTip("");
    }
}

/* ============================================================
 * 🧩 Shake animáció hibánál
 * ============================================================ */
void ModeNameDialog::shake() {
    QPoint orig = pos();
    auto* anim = new QPropertyAnimation(this, "pos");
    anim->setDuration(150);
    anim->setKeyValueAt(0.0, orig);
    anim->setKeyValueAt(0.3, orig + QPoint(-10, 0));
    anim->setKeyValueAt(0.6, orig + QPoint(10, 0));
    anim->setKeyValueAt(1.0, orig);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

/* ============================================================
 * 🧩 Visszaadott érték
 * ============================================================ */
QString ModeNameDialog::value() const {
    return _edit->text().trimmed();
}
