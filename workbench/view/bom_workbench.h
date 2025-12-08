// workbench/view/bom_workbench.h
#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QTabWidget>
#include <QVBoxLayout>

#include "products/view/product_tree_view.h"
#include "products/view/product_tree_manager.h"

/**
 * @class BOMWorkbench
 * @brief Bill of Materials Workbench – anyagszükséglet-összeállító munkapad.
 *
 * Központi elem: a terméktípus-fa (bal panel), amelyhez BOM komponensek társulnak (jobb panel).
 * - Bal oldal: ProductTreeView + ProductTreeManager (CRUD, drag&drop).
 * - Jobb oldal: QTabWidget a BOM-hoz kapcsolódó nézetekhez:
 *      - "Anyagszükséglet" (placeholder jelenleg)
 *      - "Számítási módok" (placeholder jelenleg)
 * - Állapotkezelés: splitter és fa header állapot mentése és visszaállítása.
 *
 * Elhelyezés a projektben:
 * - Mivel átível a products törzsadatokon, a workbench/ névtér alá kerül.
 * - Javasolt útvonal: workbench/view/bom_workbench.h/.cpp
 *   (Ha a .pro "bom_workbench/view"-t vár, azt egységesítsd a buildhez.)
 *
 * Integráció:
 * - A MainWindow tabjai közé illeszthető egy külön fülön ("BOM Workbench").
 * - A jelenlegi Product Types tab kiváltható vele, vagy ideiglenesen párhuzamosan futtatható.
 */
class BOMWorkbench : public QWidget {
    Q_OBJECT
public:
    explicit BOMWorkbench(QWidget* parent = nullptr);

    /**
     * @brief Visszaállítja a splitter és a fa header állapotát a SettingsManager-ből.
     */
    void restoreState();

    /**
     * @brief Elmenti a splitter és a fa header állapotát a SettingsManager-be.
     */
    void saveState();

private:
    // UI váz
    QVBoxLayout* _layout = nullptr;
    QToolBar* _toolbar = nullptr;
    QSplitter* _splitter = nullptr;

    // Bal oldal: terméktípus-fa
    ProductTreeView* _treeView = nullptr;
    ProductTreeManager* _treeManager = nullptr;

    // Jobb oldal: BOM nézetek (placeholder)
    QTabWidget* _rightTabs = nullptr;
    QWidget* _materialsTab = nullptr;    // "Anyagszükséglet"
    QWidget* _calcModesTab = nullptr;    // "Számítási módok"

    QAction* _addMaterialAction = nullptr;

    // Belső segédek
    void buildToolbar();
    void buildLeftPanel();
    void buildRightPanel();
};
