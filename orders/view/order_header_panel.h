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
    std::optional<OrderHeader> header() const;

    void clear();
private:
    QFormLayout* _layout = nullptr;

    QLineEdit* _customerName = nullptr;
    QLabel* _orderDateLabel = nullptr;
    QLineEdit* _defaultExternalPrefix = nullptr;
    QTextEdit* _note = nullptr;
    QLabel* _statusLabel = nullptr;
    QDateEdit* _deadline = nullptr;

    //std::optional<QUuid> _headerId = std::nullopt;
    std::optional<OrderHeader> _original = std::nullopt;
public:
    // ÚJ BLOKK – mező getterek
    QLineEdit* customerNameEdit() const;
    QDateEdit* deadlineEdit() const;
    QLineEdit* defaultExternalPrefixEdit() const;
    QTextEdit* noteEdit() const;

    std::optional<QUuid> headerId() const {
        return _original ? std::optional<QUuid>(_original->id) : std::nullopt;
    }

    std::optional<OrderHeader> originalHeader() const { return _original; }
};
