#include "needs/view/material_requirements_view.h"

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QGuiApplication>
#include <QClipboard>

/**
 * Hunglish megjegyzés:
 * - C++20 + Qt6.10-compatible implementáció.
 * - A view felelőssége csak a megjelenítés és UI jelek – a mentés/sync a manager/controller dolga.
 */

MaterialRequirementsView::MaterialRequirementsView(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    _table = new QTableWidget(this);
    setup_table();

    layout->addWidget(_table);
    setLayout(layout);

    // Kijelölés változás jelzése – toolbar enable/disable
    connect(_table, &QTableWidget::itemSelectionChanged,
            this, &MaterialRequirementsView::on_selection_changed);
}

void MaterialRequirementsView::setup_table() {
    _table->setColumnCount(2);
    _table->setHorizontalHeaderLabels({ "Product", "Material" });
    _table->horizontalHeader()->setStretchLastSection(true);

    // Hunglish UX: sor kijelölés, row-select az auditbarát törléshez
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    _table->setWordWrap(false);

    // Oszlopok alap szélessége – a barcode is látható legyen
    _table->setColumnWidth(0, 320); // Product
    _table->setColumnWidth(1, 360); // Material
}

void MaterialRequirementsView::set_requirements(const QVector<RequirementRow>& rows) {
    // Teljes refresh – hunglish: a manager adja a valós adatot
    _table->clearContents();
    _table->setRowCount(rows.size());

    for (int i = 0; i < rows.size(); ++i) {
        const auto& data = rows[i];

        auto* product_item  = new QTableWidgetItem(format_product_cell(data));
        auto* material_item = new QTableWidgetItem(format_material_cell(data));

        // Tooltipek – auditbarát részletek
        product_item->setToolTip(QString("Product: %1\nBarcode: %2\nUUID: %3")
                                     .arg(data.product_name, data.product_barcode, data.product_id.toString()));
        material_item->setToolTip(QString("Material: %1\nBarcode: %2\nUUID: %3")
                                      .arg(data.material_name, data.material_barcode, data.material_id.toString()));

        // Tároljuk a lényegi azonosítókat az item data-ban (repo sync-hez)
        product_item->setData(Qt::UserRole, data.product_id);
        product_item->setData(Qt::UserRole + 1, data.product_barcode);

        material_item->setData(Qt::UserRole, data.material_id);
        material_item->setData(Qt::UserRole + 1, data.material_barcode);

        _table->setItem(i, 0, product_item);
        _table->setItem(i, 1, material_item);

        apply_row_visuals(i, data);
    }
}

void MaterialRequirementsView::add_requirement(const RequirementRow& data) {
    const int row = _table->rowCount();
    _table->insertRow(row);

    auto* product_item  = new QTableWidgetItem(format_product_cell(data));
    auto* material_item = new QTableWidgetItem(format_material_cell(data));

    product_item->setToolTip(QString("Product: %1\nBarcode: %2\nUUID: %3")
                                 .arg(data.product_name, data.product_barcode, data.product_id.toString()));
    material_item->setToolTip(QString("Material: %1\nBarcode: %2\nUUID: %3")
                                  .arg(data.material_name, data.material_barcode, data.material_id.toString()));

    product_item->setData(Qt::UserRole, data.product_id);
    product_item->setData(Qt::UserRole + 1, data.product_barcode);

    material_item->setData(Qt::UserRole, data.material_id);
    material_item->setData(Qt::UserRole + 1, data.material_barcode);

    _table->setItem(row, 0, product_item);
    _table->setItem(row, 1, material_item);

    apply_row_visuals(row, data);
}

void MaterialRequirementsView::remove_selected() {
    const auto ranges = _table->selectedRanges();
    if (ranges.isEmpty()) return;

    const int row = ranges.first().topRow();
    auto* product_item  = _table->item(row, 0);
    auto* material_item = _table->item(row, 1);
    if (!product_item || !material_item) return;

    // Kibányásszuk az azonosítókat és jelezzük kifele – manager intézi a törlést
    const QUuid product_id = product_item->data(Qt::UserRole).toUuid();
    const QString product_barcode = product_item->data(Qt::UserRole + 1).toString();

    const QUuid material_id = material_item->data(Qt::UserRole).toUuid();
    const QString material_barcode = material_item->data(Qt::UserRole + 1).toString();

    emit request_remove_requirement(product_id, product_barcode, material_id, material_barcode);

    // Hunglish: a view optimista törlést végez, a manager visszatölt, ha kell
    _table->removeRow(row);
}

void MaterialRequirementsView::set_current_product(const QUuid& product_id,
                                                   const QString& product_name,
                                                   const QString& product_barcode)
{
    _current_product_id = product_id;
    _current_product_name = product_name;
    _current_product_barcode = product_barcode;

    // UX: clipboard friendly – egy kattintás, és a barcode copyzható (spacebar trükk nélkül)
    QGuiApplication::clipboard()->setText(product_barcode);

    // Hunglish: jelezzük, hogy add gomb engedélyezhető
    emit request_add_requirement(_current_product_id, _current_product_barcode);
}

void MaterialRequirementsView::apply_row_visuals(int row, const RequirementRow& data) {
    // Hiányzó anyag → piros háttér és figyelmeztető tooltip
    if (!data.material_exists) {
        if (auto* item = _table->item(row, 1)) {
            item->setBackground(QColor("#ffcccc"));
            item->setToolTip(item->toolTip() + "\nWarning: material not found in MaterialMaster!");
        }
    }
}

QString MaterialRequirementsView::format_product_cell(const RequirementRow& data) {
    // Felhasználóbarát + auditbarát – name (barcode)
    return QString("%1 (%2)").arg(data.product_name, data.product_barcode);
}

QString MaterialRequirementsView::format_material_cell(const RequirementRow& data) {
    return QString("%1 (%2)").arg(data.material_name, data.material_barcode);
}

void MaterialRequirementsView::on_selection_changed() {
    const bool has_sel = !_table->selectedRanges().isEmpty();
    emit selection_changed(has_sel);
}
