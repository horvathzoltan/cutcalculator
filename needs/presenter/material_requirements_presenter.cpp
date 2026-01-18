#include "needs/presenter/material_requirements_presenter.h"
#include "needs/registry/need_rule_registry.h"
#include "needs/view/material_picker_dialog.h"

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

    _status = new OverlayIconWidget();
    _status->setBaseEmoji("📄");
    tb->addWidget(_status);
    _view->setStatusWidget(_status);

    initialOverlay();
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

    return tb;
}

void MaterialRequirementsPresenter::initialOverlay()
{
    int repo = NeedRuleRegistry::instance().size();
    _status->updateOverlayState2(repo, 0);
}

void MaterialRequirementsPresenter::connectTreeStats()
{
    QObject::connect(_treeManager, &ProductTreeManager::treeStatsChanged,
                     this, [this](int, int) {
                         int repo = NeedRuleRegistry::instance().size();
                         _status->updateOverlayState2(repo, 0);
                     });



}

void MaterialRequirementsPresenter::refreshForProduct(const QUuid& productId,
                                                      const QString& name,
                                                      const QString& barcode)
{
    auto rows = _manager->refreshForProduct(productId, name, barcode);
    _view->set_requirements(rows);
    int repo = NeedRuleRegistry::instance().size();
    int visible = rows.size();
    _status->updateOverlayState2(repo, visible);
}
