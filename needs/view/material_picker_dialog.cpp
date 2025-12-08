#include "needs/view/material_picker_dialog.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include "materials/registry/material_registry.h"


MaterialPickerDialog::MaterialPickerDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Anyag kiválasztása");
    resize(400, 300);

    _list = new QListWidget(this);
    _okBtn = new QPushButton("OK", this);
    _cancelBtn = new QPushButton("Mégse", this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(_list);
    auto* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(_okBtn);
    btnLayout->addWidget(_cancelBtn);
    layout->addLayout(btnLayout);

    populateMaterials();

    connect(_okBtn, &QPushButton::clicked, this, &MaterialPickerDialog::onAccept);
    connect(_cancelBtn, &QPushButton::clicked, this, &MaterialPickerDialog::reject);

    _searchBox = new QLineEdit(this);
    _searchBox->setPlaceholderText("🔍 Keresés név vagy barcode alapján...");
    layout->insertWidget(0, _searchBox); // a lista fölé

    _debounceTimer = new QTimer(this);
    _debounceTimer->setSingleShot(true);
    _debounceTimer->setInterval(2000); // 2 másodperc

    connect(_searchBox, &QLineEdit::textChanged, this, [this]() {
        // minden gépelésnél újraindítjuk a timert
        _debounceTimer->start();
    });

    connect(_debounceTimer, &QTimer::timeout, this, [this]() {
        populateMaterials(); // csak akkor szűr, ha 2s-ig nem gépelt
    });
}

// void MaterialPickerDialog::populateMaterials() {
//     auto mats = MaterialRegistry::instance().readAll();
//     for (const auto& m : mats) {
//         auto* item = new QListWidgetItem(QString("%1 (%2)").arg(m.name, m.barcode));
//         item->setData(Qt::UserRole, m.id);
//         item->setData(Qt::UserRole + 1, m.barcode);
//         _list->addItem(item);
//     }
// }
void MaterialPickerDialog::populateMaterials() {
    _list->clear(); // előző lista törlése

    const QString query = _searchBox ? _searchBox->text().trimmed().toLower() : "";

    auto mats = MaterialRegistry::instance().readAll();

    QVector<QListWidgetItem*> startsWith;
    QVector<QListWidgetItem*> contains;

    for (const auto& m : mats) {
        const QString name = m.name.toLower();
        const QString barcode = m.barcode.toLower();
        const QString combined = name + " " + barcode;

        // ha van query és nem tartalmazza → kihagyjuk
        if (!query.isEmpty() && !combined.contains(query))
            continue;

        auto* item = new QListWidgetItem(QString("%1 (%2)").arg(m.name, m.barcode));
        item->setData(Qt::UserRole, m.id);
        item->setData(Qt::UserRole + 1, m.barcode);
        item->setData(Qt::UserRole + 2, m.name);

        // kezdő egyezés előre
        if (!query.isEmpty() && (name.startsWith(query) || barcode.startsWith(query)))
            startsWith.append(item);
        else
            contains.append(item);
    }

    for (auto* item : startsWith)
        _list->addItem(item);
    for (auto* item : contains)
        _list->addItem(item);
}

void MaterialPickerDialog::onAccept() {
    auto* item = _list->currentItem();
    if (!item) return;

    _result.material_id = item->data(Qt::UserRole).toUuid();
    _result.material_name = item->text().split(" (").first();
    _result.material_barcode = item->data(Qt::UserRole + 1).toString();

    accept();
}
