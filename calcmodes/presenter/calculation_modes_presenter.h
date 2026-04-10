#pragma once
#include <QObject>
#include <QToolBar>
#include <QUuid>
#include "calcmodes/view/calculation_modes_view.h"
#include "calcmodes/manager/calculation_modes_manager.h"
#include "products/view/product_tree_manager.h"
#include "ui/helpers/repository_overlay_widget.h"

class CalculationModesPresenter : public QObject {
    Q_OBJECT

public:
    explicit CalculationModesPresenter(CalculationModesView* view,
                                       CalculationModesManager* manager,
                                       ProductTreeManager* treeManager,
                                       QObject* parent = nullptr);

    QToolBar* buildToolbar(QWidget* parent);

    void refreshForProduct(const QUuid& productId,
                           const QString& name,
                           const QString& barcode);

    void refreshOverlayOnly();

signals:
    void modeSelected(std::optional<QUuid> modeId);
    void modeActivatedForDetails(QUuid modeId);

public slots:
    void on_mode_activated(const QUuid& modeId);


private:
    CalculationModesView* _view = nullptr;
    CalculationModesManager* _manager = nullptr;
    ProductTreeManager* _treeManager = nullptr;
    RepositoryOverlayWidget<NeedCalculationRegistry>* _status = nullptr;

    //void initialOverlay();
    SubscriptionToken _modeToken_1;
    SubscriptionToken _modeToken_2;

    void connectTreeStats();
    void connectRegistry();

    OverlayStatusHelper::State computeMatrixState();
    //bool isMatrixComplete() const; // v2: mátrix‑komplettség ellenőrzése eltávolítva
};
