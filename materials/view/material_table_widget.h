#pragma once

#include <QTableWidget>

/**
 * MaterialTableWidget
 *
 * Egy könnyű QTableWidget leszármazott a material viewerhez.
 * - Read-only megjelenítés
 * - Oszlopok deklarálása és alapértelmezett viselkedés beállítása
 *
 * A logika (feltöltés, cella tartalom) a MaterialTableManager-ben él.
 */
class MaterialTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit MaterialTableWidget(QWidget* parent = nullptr);

    /**
     * A megjelenített oszlopok felsorolása.
     * Az Anyag oszlopban egy egyszerű widget jelenik meg: név + színes bogyó, ha van szín.
     */
    enum Columns {
        Material = 0,      ///< Anyag név + szín-badge
        Barcode,           ///< Vonalkód
        Type,              ///< Anyagtípus megjelenítése
        Shape,             ///< Keresztmetszet és méretek
        StockLength,       ///< Szálhossz (mm)
        DefaultMachine,    ///< Alap gép
        ColorName,         ///< Szín név (ha ismert)
        CuttingMode,       ///< Vágási mód
        PaintingMode,      ///< Festési mód
        ColumnCount
    };

    /**
     * Oszlopfejlécek és szélességek beállítása.
     * A hívás a konstruktorban történik, de újrahívható ha szükséges.
     */
    void setupColumns();
};
