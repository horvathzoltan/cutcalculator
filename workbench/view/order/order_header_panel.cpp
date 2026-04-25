#include "order_header_panel.h"

OrderHeaderPanel::OrderHeaderPanel(QWidget* parent)
    : QWidget(parent)
{
    _layout = new QFormLayout(this);

    _customerName = new QLineEdit(this);
    _orderDate = new QDateEdit(QDate::currentDate(), this);
    _defaultColor = new QLineEdit(this);
    _defaultHandlerSide = new QLineEdit(this);
    _defaultExternalPrefix = new QLineEdit(this);
    _note = new QTextEdit(this);
    _status = new QComboBox(this);
    _deadline = new QDateEdit(QDate::currentDate(), this);

    _status->addItems({"New", "InProgress", "Done"});

    _layout->addRow("Customer:", _customerName);
    _layout->addRow("Order date:", _orderDate);
    _layout->addRow("Default color:", _defaultColor);
    _layout->addRow("Handler side:", _defaultHandlerSide);
    _layout->addRow("External prefix:", _defaultExternalPrefix);
    _layout->addRow("Note:", _note);
    _layout->addRow("Status:", _status);
    _layout->addRow("Deadline:", _deadline);
}
