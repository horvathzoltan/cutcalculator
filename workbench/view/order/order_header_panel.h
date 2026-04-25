#pragma once
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTextEdit>

class OrderHeaderPanel : public QWidget {
    Q_OBJECT
public:
    explicit OrderHeaderPanel(QWidget* parent = nullptr);

private:
    QFormLayout* _layout = nullptr;

    QLineEdit* _customerName = nullptr;
    QDateEdit* _orderDate = nullptr;
    QLineEdit* _defaultColor = nullptr;
    QLineEdit* _defaultHandlerSide = nullptr;
    QLineEdit* _defaultExternalPrefix = nullptr;
    QTextEdit* _note = nullptr;
    QComboBox* _status = nullptr;
    QDateEdit* _deadline = nullptr;
};
