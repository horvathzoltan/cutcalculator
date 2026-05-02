#include "handler_side_dialog.cpp"
#include "order_item_table.h"
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QKeyEvent>
#include <calcmodes/registry/need_calculation_registry.h>
#include <colors/registry/color_registry.h>
#include <products/registry/product_registry.h>

OrderItemTable::OrderItemTable(QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(8);
    setupColumns();
    setRowCount(0);
    connect(this, &QTableWidget::cellClicked, this, &OrderItemTable::onCellClicked);

    ensureEmptyRow();
}

void OrderItemTable::setupColumns()
{
    QStringList headers = {
        "External ID",
        "Product",
        "Width (mm)",
        "Height (mm)",
        "Qty",
        "Handler side",
        "Color",
        "Mode"
    };

    setHorizontalHeaderLabels(headers);
}

void OrderItemTable::setupRowWidgets(int row)
{
    // External ID
    auto* ext = new QLineEdit(this);
    setCellWidget(row, 0, ext);

    // Product combobox
    auto* product = new QComboBox(this);
    populateProductCombo(product);
    setCellWidget(row, 1, product);


    // Width
    auto* w = new QSpinBox(this);
    w->setRange(1, 100000);
    setCellWidget(row, 2, w);

    // Height
    auto* h = new QSpinBox(this);
    h->setRange(1, 100000);
    setCellWidget(row, 3, h);

    // Qty
    auto* qty = new QSpinBox(this);
    qty->setRange(1, 999);
    setCellWidget(row, 4, qty);

    // Handler side (PATCH 6 fogja kezelni)
    auto* hs = new QLineEdit(this);
    hs->setReadOnly(true);
    setCellWidget(row, 5, hs);

    // Color combobox
    auto* color = new QComboBox(this);
    populateColorCombo(color);
    setCellWidget(row, 6, color);


    // Mode combobox
    auto* mode = new QComboBox(this);
    setCellWidget(row, 7, mode);

    // Product változás → Mode lista frissítése
    product = qobject_cast<QComboBox*>(cellWidget(row, 1));
    if (product) {
        connect(product, &QComboBox::currentIndexChanged, this,
                [this, row](int) {
                    auto* p = qobject_cast<QComboBox*>(cellWidget(row, 1));
                    auto* m = qobject_cast<QComboBox*>(cellWidget(row, 7));
                    if (!p || !m)
                        return;

                    QUuid pid = p->currentData(Qt::UserRole).toUuid();
                    populateModeCombo(m, pid);
                });
    }

}

void OrderItemTable::addEmptyRow()
{
    int row = rowCount();
    insertRow(row);
    setupRowWidgets(row);
}

void OrderItemTable::onCellClicked(int row, int column)
{
    if (column != 5)
        return;

    auto* qtyWidget = qobject_cast<QSpinBox*>(cellWidget(row, 4));
    if (!qtyWidget)
        return;

    int qty = qtyWidget->value();

    auto* hsWidget = qobject_cast<QLineEdit*>(cellWidget(row, 5));
    if (!hsWidget)
        return;

    QString text = hsWidget->text();
    int jInit = 0;
    int bInit = 0;

    if (text.contains("J") && text.contains("B")) {
        auto parts = text.split("-");
        if (parts.size() == 2) {
            jInit = parts[0].remove("J").toInt();
            bInit = parts[1].remove("B").toInt();
        }
    }

    HandlerSideDialog dlg(qty, jInit, bInit, this);
    if (dlg.exec() == QDialog::Accepted) {
        QString result = QString("%1J-%2B").arg(dlg.j()).arg(dlg.b());
        hsWidget->setText(result);
    }

    ensureEmptyRow();

}

void OrderItemTable::setItems(const QVector<OrderItem>& items)
{
    setRowCount(0);

    for (const auto& it : items) {
        int row = rowCount();
        insertRow(row);
        setupRowWidgets(row);

        // External ID
        if (auto* w = qobject_cast<QLineEdit*>(cellWidget(row, 0)))
            w->setText(it.externalId);

        // Product (UUID → combobox)
        if (auto* w = qobject_cast<QComboBox*>(cellWidget(row, 1))) {
            int idx = w->findData(it.productId, Qt::UserRole);
            if (idx >= 0)
                w->setCurrentIndex(idx);
        }

        // Width
        if (auto* w = qobject_cast<QSpinBox*>(cellWidget(row, 2)))
            w->setValue(it.width_mm);

        // Height
        if (auto* w = qobject_cast<QSpinBox*>(cellWidget(row, 3)))
            w->setValue(it.height_mm);

        // Qty
        if (auto* w = qobject_cast<QSpinBox*>(cellWidget(row, 4)))
            w->setValue(it.order_qty);

        // Handler side
        if (auto* w = qobject_cast<QLineEdit*>(cellWidget(row, 5)))
            w->setText(it.handlerSide);

        // Color
        if (auto* w = qobject_cast<QComboBox*>(cellWidget(row, 6))) {
            int idx = w->findText(it.colorName);
            if (idx >= 0)
                w->setCurrentIndex(idx);
        }

        // Mode
        // Mode
        if (auto* m = qobject_cast<QComboBox*>(cellWidget(row, 7))) {
            // előbb fel kell tölteni a productId alapján
            if (auto* p = qobject_cast<QComboBox*>(cellWidget(row, 1))) {
                QUuid pid = p->currentData(Qt::UserRole).toUuid();
                populateModeCombo(m, pid);
            }
            m->setCurrentText(it.modeName);
        }


        // ID (IdentifiableEntity)
        item(row, 0)->setData(Qt::UserRole + 100, it.id);
    }
        ensureEmptyRow();   // ⭐ ÚJ
}

QVector<OrderItem> OrderItemTable::toItems(const QUuid& orderId,
                                           const QString& ownerName) const
{
    QVector<OrderItem> out;

    for (int row = 0; row < rowCount(); ++row) {
        OrderItem it;

        // ID (ha nincs, új UUID)
        QVariant idv = item(row, 0)->data(Qt::UserRole + 100);
        it.id = idv.isValid() ? idv.toUuid() : QUuid::createUuid();

        it.orderId = orderId;
        it.ownerName = ownerName;

        // External ID
        if (auto* w = qobject_cast<QLineEdit*>(cellWidget(row, 0)))
            it.externalId = w->text();

        // Product (UUID)
        if (auto* w = qobject_cast<QComboBox*>(cellWidget(row, 1)))
            it.productId = w->currentData(Qt::UserRole).toUuid();

        // Width
        if (auto* w = qobject_cast<QSpinBox*>(cellWidget(row, 2)))
            it.width_mm = w->value();

        // Height
        if (auto* w = qobject_cast<QSpinBox*>(cellWidget(row, 3)))
            it.height_mm = w->value();

        // Qty
        if (auto* w = qobject_cast<QSpinBox*>(cellWidget(row, 4)))
            it.order_qty = w->value();

        // Handler side
        if (auto* w = qobject_cast<QLineEdit*>(cellWidget(row, 5)))
            it.handlerSide = w->text();

        // Color
        if (auto* w = qobject_cast<QComboBox*>(cellWidget(row, 6)))
            it.colorName = w->currentText();

        // Mode
        if (auto* w = qobject_cast<QComboBox*>(cellWidget(row, 7)))
            it.modeName = w->currentText();

        // Üres sor kihagyása
        if (it.productId.isNull() && it.externalId.isEmpty())
            continue;

        out.append(it);
    }

    return out;
}

void OrderItemTable::populateProductCombo(QComboBox* combo)
{
    combo->clear();

    const auto all = ProductRegistry::instance().readAll();
    for (const auto& p : all) {
        combo->addItem(
            p.name,            // display
            p.id               // stored UUID
            );
    }
}

void OrderItemTable::populateModeCombo(QComboBox* combo, const QUuid& productId)
{
    combo->clear();

    const auto modes =
        NeedCalculationRegistry::instance().findAll(
            [&](const NeedCalculation& nc) {
                return nc.productId == productId;
            });

    for (const auto& m : modes) {
        combo->addItem(m.name);   // domain mező: modeName (string)
    }
}

void OrderItemTable::populateColorCombo(QComboBox* combo)
{
    combo->clear();

    const auto all = ColorRegistry::instance().readAll();
    for (const auto& c : all) {
        combo->addItem(c.name());   // domain mező: colorName (string)
    }
}

void OrderItemTable::ensureEmptyRow()
{
    if (rowCount() == 0) {
        addEmptyRow();
        return;
    }

    // Utolsó sor üres?
    bool empty = true;

    for (int col = 0; col < columnCount(); ++col) {
        QWidget* w = cellWidget(rowCount() - 1, col);
        if (!w)
            continue;

        if (auto* le = qobject_cast<QLineEdit*>(w)) {
            if (!le->text().isEmpty())
                empty = false;
        }
        else if (auto* sb = qobject_cast<QSpinBox*>(w)) {
            if (sb->value() != 0)
                empty = false;
        }
        else if (auto* cb = qobject_cast<QComboBox*>(w)) {
            if (cb->currentIndex() != -1)
                empty = false;
        }
    }

    if (!empty)
        addEmptyRow();
}

void OrderItemTable::deleteRowAt(int row)
{
    if (row < 0 || row >= rowCount())
        return;

    removeRow(row);
    ensureEmptyRow();
}

void OrderItemTable::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Delete) {
        auto items = selectedItems();
        if (!items.isEmpty()) {
            int row = items.first()->row();
            deleteRowAt(row);
            return;
        }
    }

    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        addEmptyRow();
        return;
    }

    QTableWidget::keyPressEvent(e);
}

