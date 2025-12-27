// workbench/view/bom_workbench.h
#pragma once
#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QVBoxLayout>

#include "needs/view/material_requirements_view.h"
#include "products/view/product_tree_view.h"
#include "products/view/product_tree_manager.h"
#include "calcmodes/view/calculation_modes_view.h"
#include "calculation/view/calculation_mode_detail_view.h"
#include "needs/manager/material_requirements_manager.h"
#include "calcmodes/manager/calculation_modes_manager.h"
#include "calculation/manager/calculation_mode_detail_manager.h"

/**
 * @class BOMWorkbench
 * @brief Bill of Materials Workbench – anyagszükséglet + számítási módok + formulák közös munkapad.
 *
 * Új layout:
 * - Bal oldal: ProductTreeView + ProductTreeManager (CRUD, drag&drop).
 * - Jobb oldal: fő splitter (horizontal):
 *      - Bal vertical splitter:
 *          - Felső: MaterialRequirementsView (NeedRules)
 *          - Alsó: CalculationModesView (NeedCalculations)
 *      - Jobb: CalculationModeDetailView (NeedCalculationDetails)
 *
 * Állapotkezelés: splitter és fa header állapot mentése és visszaállítása.
 * Integráció: MainWindow tabjai közé illeszthető külön fülön ("BOM Workbench").
 */
class BOMWorkbench : public QWidget {
    Q_OBJECT
public:
    explicit BOMWorkbench(QWidget* parent = nullptr);

    /// Visszaállítja a splitter és a fa header állapotát a SettingsManager-ből.
    void restoreState();

    /// Elmenti a splitter és a fa header állapotát a SettingsManager-be.
    void saveState();

private:
    // UI váz
    QVBoxLayout* _layout = nullptr;
    QSplitter* _splitter = nullptr;              // fő horizontal splitter (bal fa + jobb panel)
    QSplitter* _rightMainSplitter = nullptr;     // jobb oldali horizontal splitter
    QSplitter* _leftVerticalSplitter = nullptr;  // bal oldali vertical splitter (NeedRules + Modes)

    // Bal oldal: terméktípus-fa
    ProductTreeView* _treeView = nullptr;
    ProductTreeManager* _treeManager = nullptr;

    // Jobb oldal: három view
    MaterialRequirementsView* _matView = nullptr;
    CalculationModesView* _modesView = nullptr;
    CalculationModeDetailView* _detailView = nullptr;

    // Managerek
    MaterialRequirementsManager* _matManager = nullptr;
    CalculationModesManager* _modesManager = nullptr;
    CalculationModeDetailManager* _detailManager = nullptr;

    // Toolbars (opcionális)
    QToolBar* _treeToolbar = nullptr;
    QToolBar* _matToolbar = nullptr;
    QToolBar* _modesToolbar = nullptr;
    QToolBar* _detailsToolbar = nullptr;

    // Belső segédek
    void buildLeftPanel();
    void buildRightPanel();
    QToolBar* buildTreeToolbar(QWidget* parent);
    QToolBar* buildMaterialToolbar(QWidget* parent, MaterialRequirementsView* mat_view);
    QToolBar* buildModesToolbar(QWidget* parent, CalculationModesView* modes_view);
    QToolBar* buildDetailsToolbar(QWidget* parent, CalculationModeDetailView* detail_view);

    //bool event(QEvent *e) override;
    void showEvent(QShowEvent* event) override;

    bool _restoredOnce = false;
    bool _isFullyShown = false;


   // void closeEvent(QCloseEvent *event) override;

};
