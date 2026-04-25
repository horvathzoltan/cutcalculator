#pragma once
#include <QTableWidget>

class OrderItemTable : public QTableWidget {
    Q_OBJECT
public:
    explicit OrderItemTable(QWidget* parent = nullptr);

    void setupColumns();
};
