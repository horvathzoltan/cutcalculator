#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QUuid>

class OrderHeaderListPanel : public QWidget {
    Q_OBJECT
public:
    explicit OrderHeaderListPanel(QWidget* parent = nullptr);

    std::optional<QUuid> selectedOrderId() const;
    void selectById(const QUuid& id);
    void silentSelectById(const QUuid& id);

    void refresh();

    int visibleRowCount() const;
    QUuid nextOrderIdAfter(const QUuid &id) const;
signals:
    void orderSelected(const QUuid& id);

private slots:
    void onFilterChanged();

private:
    QVBoxLayout* _layout = nullptr;

    // Filter mezők
    QLineEdit* _filterCustomer = nullptr;
    QDateEdit* _filterFrom = nullptr;
    QDateEdit* _filterTo = nullptr;
    QComboBox* _filterStatus = nullptr;

    // Lista
    QTableWidget* _table = nullptr;

    void rebuildList();
};
