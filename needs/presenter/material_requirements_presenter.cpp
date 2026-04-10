#include "needs/presenter/material_requirements_presenter.h"
#include "needs/registry/need_rule_registry.h"
#include "needs/view/material_picker_dialog.h"
#include <calcmodes/registry/need_calculation_registry.h>
#include <calculation/registry/need_calculation_detail_registry.h>
#include "calculation/service/matrix_validator.h"

MaterialRequirementsPresenter::MaterialRequirementsPresenter(
    MaterialRequirementsView* view,
    MaterialRequirementsManager* manager,
    ProductTreeManager* treeManager,
    QObject* parent)
    : QObject(parent)
    , _view(view)
    , _manager(manager)
    , _treeManager(treeManager)
{
    QObject::connect(_treeManager, &ProductTreeManager::currentProductChanged,
                     this, [this](const QUuid& id,
                            const QString& name,
                            const QString& barcode) {
                         refreshForProduct(id, name, barcode);
                     });
}

QToolBar* MaterialRequirementsPresenter::buildToolbar(QWidget* parent)
{
    auto* tb = new QToolBar("Anyagszükséglet", parent);

    // _status = new OverlayIconWidget();
    // _status->setBaseEmoji("📄");
    // tb->addWidget(_status);
    // _view->setStatusWidget(_status);
    // _status->setObjectName("MaterialOverlay");

    _status = new RepositoryOverlayWidget<NeedRuleRegistry>(tb, "MaterialRequirementsOverlay");
    //_view->setStatusWidget(_status);

    refreshOverlayOnly();
    connectTreeStats();

    QAction* addAct    = tb->addAction("➕ Hozzáadás");
    QAction* removeAct = tb->addAction("🗑️ Törlés");

    QObject::connect(addAct, &QAction::triggered, this, [this]() {
        MaterialPickerDialog dlg(_view);
        if (dlg.exec() != QDialog::Accepted)
            return;

        auto result = dlg.result();   // result.material_id

        emit _view->request_add_requirement(_treeManager->currentProductId(), result.material_id);

    });

    QObject::connect(removeAct, &QAction::triggered, this, [this]() {
        _view->remove_selected();
    });

    connectRegistry();
    return tb;
}

void MaterialRequirementsPresenter::connectTreeStats()
{
    QObject::connect(_treeManager, &ProductTreeManager::treeStatsChanged,
                     this, [this]() { refreshOverlayOnly(); });
}

void MaterialRequirementsPresenter::refreshForProduct(const QUuid& productId,
                                                      const QString& name,
                                                      const QString& barcode)
{
    auto rows = _manager->refreshForProduct(productId, name, barcode);
    _view->set_requirements(rows);

    // 🔥 Stabilizált overlay frissítés
    QMetaObject::invokeMethod(this, [this]() {
        refreshOverlayOnly();
    }, Qt::QueuedConnection);

}

void MaterialRequirementsPresenter::refreshOverlayOnly()
{
    auto state = computeMatrixState();
    _status->setState(state);
}



void MaterialRequirementsPresenter::connectRegistry() {
    _needRuleToken_1 =
        NeedRuleRegistry::instance().subscribeItemsChangedToken(
            [this]() {
                auto id = _treeManager->currentProductId();
                if(!id.isNull()) {
                    auto name = _treeManager->currentProductName();
                    auto barcode = _treeManager->currentProductBarcode();
                    refreshForProduct(id, name, barcode);
                }

                refreshOverlayOnly();
            });

    // _needRuleToken_2 =
    //     NeedCalculationDetailRegistry::instance().subscribeItemsChangedToken(
    //      [this]() { refreshOverlayOnly(); });
}


OverlayStatusHelper::State MaterialRequirementsPresenter::computeMatrixState()
{
    int repo = NeedRuleRegistry::instance().size();
    int visible = _view->rowCount();

    if (repo == 0)
        return OverlayStatusHelper::State::EmptyRepo;

    if (visible == 0)
        return OverlayStatusHelper::State::NoVisibleRows;

    // 🔵 v3: mátrix-komplettség ellenőrzése
    // const QUuid productId = _treeManager->currentProductId();
    // bool complete = MatrixValidator::isProductMatrixComplete(productId);

    // if (!complete)
    //     return OverlayStatusHelper::State::Incomplete;

    return OverlayStatusHelper::State::Normal;
}


// bool MaterialRequirementsPresenter::isMatrixComplete() const
// {
//     const auto rules = NeedRuleRegistry::instance().readAll();
//     const auto modes = NeedCalculationRegistry::instance().readAll();

//     for (const auto& r : rules) {
//         for (const auto& m : modes) {
//             if (m.productId != r.leftId)
//                 continue;

//             bool exists = NeedCalculationDetailRegistry::instance()
//                               .existsBy([&](const NeedCalculationDetail& d){
//                                   return d.needCalculationId == m.id &&
//                                          d.materialId == r.rightId;
//                               });

//             if (!exists)
//                 return false;
//         }
//     }
//     return true;
// }
