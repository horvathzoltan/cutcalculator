#include "materials/view/material_table_manager.h"
#include "common/utils/font_utils.h"
#include "materials/view/color_badge.h"
#include "materials/view/material_table_widget.h"

#include "materials/registry/material_registry.h"
#include "materials/model/material_master.h"
#include "materials/model/material_type.h"
#include "materials/model/cutting_mode.h"
#include "materials/model/painting_mode.h"
#include "materials/model/crosssectionshape.h"

#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

/**
 * Konstruktor: csak a táblára hivatkozik, logika a populateAll-ban.
 */
MaterialTableManager::MaterialTableManager(MaterialTableWidget* table, QObject* parent)
    : QObject(parent), _table(table) {}

void MaterialTableManager::clear() {
    if (!_table) return;
    _table->clearContents();
    _table->setRowCount(0);
}

void MaterialTableManager::populateAll() {
    if (!_table) return;
    clear();

    const auto& all = MaterialRegistry::instance().readAll();
    _table->setRowCount(all.size());

    for (int i = 0; i < all.size(); ++i)
        addMaterialRow(all[i], i);
}

// void MaterialTableManager::refresh() {
//     populateAll();
// }

/**
 * Egy sor feltöltése: Anyag widget + egyéb oszlopok fromText itemek.
 */
void MaterialTableManager::addMaterialRow(const MaterialMaster& m, int rowIx) {
    // Anyag cella (név + szín-bogyó), csoport színezés nélkül
    {
        const QString tt = materialTooltip(m);
        QWidget* w = createMaterialCellWidget(m, tt);
        _table->setCellWidget(rowIx, MaterialTableWidget::Material, w);
    }

    auto setText = [&](int col, const QString& text) {
        auto* item = new QTableWidgetItem(text);
        item->setToolTip(text);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        _table->setItem(rowIx, col, item);
    };

    // Barcode
    setText(MaterialTableWidget::Barcode, m.barcode);

    // Típus
    setText(MaterialTableWidget::Type, m.type.toString());

    // Forma (kiegészítve méretekkel / átmérővel)
    setText(MaterialTableWidget::Shape, shapeDisplay(m));

    // Szálhossz mm
    setText(MaterialTableWidget::StockLength, QString::number(m.stockLength_mm));

    // Alap gép
    setText(MaterialTableWidget::DefaultMachine, m.defaultMachineId);

    // Szín
    setText(MaterialTableWidget::ColorName, colorDisplay(m));

    // Vágási mód
    setText(MaterialTableWidget::CuttingMode, cuttingModeDisplay(m));

    // Festési mód
    setText(MaterialTableWidget::PaintingMode, paintingModeDisplay(m));
}

/**
 * Egyszerű tooltip, kizárólag a viewerhez – csoport nélkül.
 */
QString MaterialTableManager::materialTooltip(const MaterialMaster& m) const {
    const QString colorName = m.color.isValid() ? m.color.name() : "—";
    return QString("Anyag: %1\nBarcode: %2\nColor: %3")
        .arg(m.name,
             m.barcode.isEmpty() ? "—" : m.barcode,
             colorName);
}

/**
 * Anyag cella widget: balra név, utána kicsi színes badge, ha van érvényes szín.
 * Nem használ group-színezést; csak a saját szín bogyó jelenik meg.
 */
// QWidget* MaterialTableManager::createMaterialCellWidget(const MaterialMaster& m, const QString& tooltip) const {
//     QWidget* panel = new QWidget();
//     auto* layout = new QHBoxLayout(panel);
//     layout->setContentsMargins(2, 0, 0, 0);
//     layout->setSpacing(2);
//     layout->setAlignment(Qt::AlignLeft);

//     auto* nameLabel = new QLabel(m.name);
//     nameLabel->setToolTip(tooltip);
//     layout->addWidget(nameLabel);

//     if (m.color.isValid()) {
//         const QColor color = m.color.color();
//         const QColor fg = color.lightness() < 128 ? Qt::white : Qt::black;
//         auto metrics = FontUtils::getCharMetrics(panel, 'W');
//         int badgeSize = metrics.height;// * 0.8; // pl. 80%-a a betűmagasságnak

//         auto* colorBox = new QFrame();
//         colorBox->setToolTip(QString("Anyag színe: %1").arg(m.color.name()));
//         colorBox->setFixedSize(badgeSize, badgeSize);
//         // colorBox->setStyleSheet(QString(
//         //                             "background-color: %1; color: %2;"
//         //                             "border-radius: %3px;"
//         //                             "border: 1px solid #888;"
//         //                             "QToolTip { background-color: #ffffe0; color: black; border: 1px solid gray; }"
//         //                             ).arg(color.name(), fg.name()).arg(badgeSize / 2));

//         colorBox->setStyleSheet(QString(
//                                     "background-color: %1;"
//                                     "border-radius: %2px;"
//                                     "border: 1px solid #888;"
//                                     ).arg(color.name()).arg(badgeSize / 2));

//         layout->addWidget(colorBox);
//     }

//     panel->setLayout(layout);
//     return panel;
// }
QWidget* MaterialTableManager::createMaterialCellWidget(const MaterialMaster& m, const QString& tooltip) const {
    QWidget* panel = new QWidget();
    auto* layout = new QHBoxLayout(panel);
    layout->setContentsMargins(2, 0, 0, 0);
    layout->setSpacing(2);
    layout->setAlignment(Qt::AlignLeft);

    auto* nameLabel = new QLabel(m.name);
    nameLabel->setToolTip(tooltip);
    layout->addWidget(nameLabel);

    if (m.color.isValid()) {
        const QColor color = m.color.color();
        auto metrics = FontUtils::getCharMetrics(panel, 'W');
        int badgeSize = metrics.height * 0.8;//75;
        auto tooltiptxt = QString("Anyag színe: %1").arg(m.color.name());
        auto* badge = new ColorBadge(color, tooltiptxt, badgeSize, panel);
        layout->addWidget(badge);
    }

    panel->setLayout(layout);
    return panel;
}
/**
 * Forma megjelenítés: Round → Ød mm, Rectangular → w × h mm, egyéb → toString().
 */
QString MaterialTableManager::shapeDisplay(const MaterialMaster& m) const {
    const auto shape = m.shape.value;

    if (shape == CrossSectionShape::Shape::Rectangular) {
        return QString("%1 × %2 mm").arg(m.size_mm.width()).arg(m.size_mm.height());
    }
    if (shape == CrossSectionShape::Shape::Round) {
        return QString("Ø %1 mm").arg(m.diameter_mm);
    }
    return m.shape.toString();
}

QString MaterialTableManager::colorDisplay(const MaterialMaster& m) const {
    return m.color.isValid() ? m.color.name() : "Nincs szín";
}

QString MaterialTableManager::cuttingModeDisplay(const MaterialMaster& m) const {
    return CuttingModeUtils::toString(m.cuttingMode);
}

QString MaterialTableManager::paintingModeDisplay(const MaterialMaster& m) const {
    return PaintingModeUtils::toString(m.paintingMode);
}
