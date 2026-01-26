#include "material_picker_dialog.h"
#include "materials/registry/material_registry.h"
#include <QVBoxLayout>
#include <QListWidget>
#include <QDialogButtonBox>

MaterialPickerDialog::MaterialPickerDialog(QWidget* parent)
    : QDialog(parent)
{
    auto* layout = new QVBoxLayout(this);

    _list = new QListWidget(this);
    layout->addWidget(_list);

    for (const auto& m : MaterialRegistry::instance().readAll()) {
        auto* item = new QListWidgetItem(QString("%1 (%2)")
                                             .arg(m.name, m.barcode));
        item->setData(Qt::UserRole, m.id);
        _list->addItem(item);
    }

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        auto* item = _list->currentItem();
        if (item)
            _selected = item->data(Qt::UserRole).toUuid();
        accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QUuid MaterialPickerDialog::selectedMaterial() const {
    return _selected;
}
