#pragma once
#include "orders/model/order_header.h"
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>

class OrderHeaderPanel : public QWidget {
    Q_OBJECT
public:
    explicit OrderHeaderPanel(QWidget* parent = nullptr);

    void setHeader(const OrderHeader& h);
    OrderHeader toHeader(const QUuid& id) const;

    void clear();
private:
    QFormLayout* _layout = nullptr;

    QLineEdit* _customerName = nullptr;
    QLabel* _orderDateLabel = nullptr;
    QLineEdit* _defaultExternalPrefix = nullptr;
    QTextEdit* _note = nullptr;
    QLabel* _statusLabel = nullptr;
    QDateEdit* _deadline = nullptr;
};
