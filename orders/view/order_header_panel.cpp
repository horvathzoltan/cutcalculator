#include "order_header_panel.h"

OrderHeaderPanel::OrderHeaderPanel(QWidget* parent)
    : QWidget(parent)
{
    _layout = new QFormLayout(this);

    _customerName = new QLineEdit(this);
    _defaultExternalPrefix = new QLineEdit(this);
    _note = new QTextEdit(this);
    _deadline = new QDateEdit(QDate::currentDate(), this);
    _orderDateLabel = new QLabel(QDate::currentDate().toString(Qt::ISODate), this);
    _statusLabel = new QLabel("NEW", this);

    // UI sorrend helyreállítása
    _layout->addRow("Customer:", _customerName);              // ⭐ HIÁNYZOTT
    _layout->addRow("Order date:", _orderDateLabel);
    _layout->addRow("Deadline:", _deadline);
    _layout->addRow("External prefix:", _defaultExternalPrefix);
    _layout->addRow("Note:", _note);
    _layout->addRow("Status:", _statusLabel);

    _deadline->setStyleSheet("font-size: 18px; font-weight: bold;");
}

void OrderHeaderPanel::setHeader(const OrderHeader& h)
{
    _customerName->setText(h.customerName);
    _orderDateLabel->setText(h.orderDate.toString(Qt::ISODate));
    _deadline->setDate(h.deadline);
    _defaultExternalPrefix->setText(h.defaultExternalPrefix);
    _note->setPlainText(h.note);
    _statusLabel->setText(h.status);

    _original = h;
    //_headerId = h.id;
}

std::optional<OrderHeader> OrderHeaderPanel::header() const
{
    if (!_original)
        return std::nullopt;

    OrderHeader h = *_original;
    h.customerName = _customerName->text();

    QString txt = _orderDateLabel->text();
    h.orderDate = txt.isEmpty() ? QDate() : QDate::fromString(txt, Qt::ISODate);

    //h.orderDate = QDate::fromString(_orderDateLabel->text(), Qt::ISODate);
    h.deadline = _deadline->date();
    h.defaultExternalPrefix = _defaultExternalPrefix->text();
    h.note = _note->toPlainText();
    h.status = _statusLabel->text();
    return h;
}


void OrderHeaderPanel::clear()
{
    _original.reset();

    _customerName->clear();
    _orderDateLabel->setText("");
    _deadline->setDate(QDate()); // üres dátum
    _defaultExternalPrefix->clear();
    _note->clear();
    _statusLabel->clear();
}

QLineEdit* OrderHeaderPanel::customerNameEdit() const { return _customerName; }
QDateEdit* OrderHeaderPanel::deadlineEdit() const { return _deadline; }
QLineEdit* OrderHeaderPanel::defaultExternalPrefixEdit() const { return _defaultExternalPrefix; }
QTextEdit* OrderHeaderPanel::noteEdit() const { return _note; }
