#include "products/dialogs/product_name_dialog.h"
#include "products/validation/product_validation.h"
#include <QCursor>
#include <QStyle>
#include <QTimer>

ProductNameDialog::ProductNameDialog(QWidget* parent,
                                     const QString& initialName,
                                     const QString& initialBarcode,
                                     const QUuid& parentId,
                                     std::function<bool(const QString&, QString&)> nameCheck,
                                     std::function<bool(const QString&, QString&)> barcodeCheck)
    : QDialog(parent)
    , _parentId(parentId)
    , _nameCheck(std::move(nameCheck))
    , _barcodeCheck(std::move(barcodeCheck))
{
    setWindowTitle("Új terméktípus");
    setModal(true);
    setMinimumWidth(320);

    auto* layout = new QVBoxLayout(this);

    // A dialógus két mezőt kezel: név + vonalkód.
    // A validáció callbackeken keresztül történik (UI dry-run).

    // --- NÉV ---
    _label = new QLabel("Add meg a terméktípus nevét:", this);

    layout->addWidget(_label);

    _edit = new QLineEdit(this);
    _edit->setText(initialName);
    _edit->selectAll();
    layout->addWidget(_edit);

    connect(_edit, &QLineEdit::textChanged,
            this, &ProductNameDialog::onNameChanged);

    // --- BARCODE ---
    auto* barcodeLabel = new QLabel("Vonalkód:", this);
    layout->addWidget(barcodeLabel);

    _barcodeEdit = new QLineEdit(this);
    _barcodeEdit->setText(initialBarcode);
    layout->addWidget(_barcodeEdit);

    connect(_barcodeEdit, &QLineEdit::textChanged,
            this, &ProductNameDialog::onBarcodeChanged);

    // --- GLOBÁLIS HIBA LABEL ---
    _globalError = new QLabel(this);
    _globalError->setStyleSheet("color: red; font-weight: bold;");
    _globalError->setWordWrap(true);
    layout->addWidget(_globalError);

    // --- BUTTONS ---
    _buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                    Qt::Horizontal,
                                    this);
    _ok = _buttons->button(QDialogButtonBox::Ok);
    layout->addWidget(_buttons);

    connect(_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // --- UX ---
    QTimer::singleShot(0, this, [this]() {
        CursorAwarePlacement::placeNearCursor(this);
    });

    _blinker = new AttentionBlinker(this);
    _blinker->attach(this);
    QTimer::singleShot(1200, this, [this]() {
        if (!_attentionRemoved) {
            _blinker->start(AttentionBlinker::Light, AttentionBlinker::GrayMode);
        }
    });

    // --- KEZDETI VALIDÁCIÓ ---
    updateOkState();
}

void ProductNameDialog::onNameChanged(const QString&)
{
    updateOkState();
}

void ProductNameDialog::onBarcodeChanged(const QString&)
{
    updateOkState();
}

void ProductNameDialog::updateOkState()
{
    QString err;

    const QString name = _edit->text().trimmed();
    const QString code = _barcodeEdit->text().trimmed();

    // 1) Név validáció
    if (!_nameCheck(name, err)) {
        if (_globalError->text() != err)
            _globalError->setText(err);
        _ok->setEnabled(false);
        return;
    }

    // 2) Barcode validáció
    if (!_barcodeCheck(code, err)) {
        if (_globalError->text() != err)
            _globalError->setText(err);
        _ok->setEnabled(false);
        return;
    }

    // 3) Minden OK
    if (!_globalError->text().isEmpty())
        _globalError->clear();

    _ok->setEnabled(true);
}

QString ProductNameDialog::value() const
{
    return _edit->text().trimmed();
}

QString ProductNameDialog::barcodeValue() const
{
    return _barcodeEdit->text().trimmed();
}

