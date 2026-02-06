#pragma once
#include <QObject>
#include <QToolBar>
#include <QUuid>
#include "needs/view/material_requirements_view.h"
#include "needs/manager/material_requirements_manager.h"
#include "products/view/product_tree_manager.h"
#include "ui/helpers/repository_overlay_widget.h"

class OverlayPresenterTester; // forward

class MaterialRequirementsPresenter : public QObject {
    Q_OBJECT

    //friend class OverlayPresenterTester; // <<< EZ A LÉNYEG

public:
    explicit MaterialRequirementsPresenter(MaterialRequirementsView* view,
                                           MaterialRequirementsManager* manager,
                                           ProductTreeManager* treeManager,
                                           QObject* parent = nullptr);

    // Teszt konstruktor
    // MaterialRequirementsPresenter() : _view(nullptr), _manager(nullptr), _treeManager(nullptr), _status(nullptr)
    // {}

    QToolBar* buildToolbar(QWidget* parent);

    void refreshForProduct(const QUuid& productId,
                           const QString& name,
                           const QString& barcode);

    void refreshOverlayOnly();
private:

    MaterialRequirementsView* _view = nullptr;
    MaterialRequirementsManager* _manager = nullptr;
    ProductTreeManager* _treeManager = nullptr;
    RepositoryOverlayWidget<NeedRuleRegistry>* _status = nullptr;
    SubscriptionToken _needRuleToken; // <<< ÚJ

    void connectTreeStats();
    void connectRegistry();
    OverlayStatusHelper::State computeMatrixState();

    // v2: mátrix‑komplettség ellenőrzése eltávolítva
    //bool isMatrixComplete() const;
};
