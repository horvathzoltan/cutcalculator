#include "materials/view/material_table_widget.h"
#include "common/utils/font_utils.h"
#include <QHeaderView>

MaterialTableWidget::MaterialTableWidget(QWidget* parent)
    : QTableWidget(parent)
{
    FontUtils::applySafeMonospaceFont(this);
    verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 3);

    setColumnCount(ColumnCount);
    setupColumns();

    // Táblázat viselkedés
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    setWordWrap(false);
}

void MaterialTableWidget::setupColumns() {
    setHorizontalHeaderLabels({
        "Anyag", "Barcode", "Típus", "Forma",
        "Szálhossz (mm)", "Gép", "Szín", "Vágás mód", "Festés mód"
    });

    // Praktikus alap szélességek; finomhangolható.
    setColumnWidth(Material,      280);
    setColumnWidth(Barcode,       160);
    setColumnWidth(Type,          140);
    setColumnWidth(Shape,         160);
    setColumnWidth(StockLength,   140);
    setColumnWidth(DefaultMachine,140);
    setColumnWidth(ColorName,     140);
    setColumnWidth(CuttingMode,   140);
    // PaintingMode (last section) automatikusan nyúlik
}
