#pragma once
#include "orders/model/order_header.h"
#include "orders/model/order_item.h"
#include <QComboBox>
#include <QTableWidget>

class OrderItemTable : public QTableWidget {
    Q_OBJECT
public:
    explicit OrderItemTable(QWidget* parent = nullptr);

    void setupColumns();
    void setItems(const QVector<OrderItem>& items);
    QVector<OrderItem> toItems(const QUuid& orderId, const QString& ownerName) const;

    void setupRowWidgets(int row);
    void populateProductCombo(QComboBox* combo);
    void populateModeCombo(QComboBox* combo, const QUuid& productId);
    void populateColorCombo(QComboBox* combo);

private:
    void addEmptyRow();
    void ensureEmptyRow();          // ⭐ ÚJ
    void deleteRowAt(int row);      // ⭐ ÚJ

protected:
    void keyPressEvent(QKeyEvent* e) override;   // ⭐ ÚJ

private slots:
    void onCellClicked(int row, int column);

};
