#include "calcmodes/presenter/calculation_modes_presenter.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/dialogs/mode_name_dialog.h"

#include <ui/helpers/repository_overlay_widget.h>
#include <ui/helpers/overlay_status_helper.h>

CalculationModesPresenter::CalculationModesPresenter(
    CalculationModesView* view,
    CalculationModesManager* manager,
    ProductTreeManager* treeManager,
    QObject* parent)
    : QObject(parent)
    , _view(view)
    , _manager(manager)
    , _treeManager(treeManager)
{
    connect(_treeManager, &ProductTreeManager::currentProductChanged,
            this, [this](const QUuid& id, const QString& name, const QString& barcode) {
                _view->set_modes({});
                refreshOverlayOnly();
                if (_view->isReady())
                    refreshForProduct(id, name, barcode);
                emit modeSelected(std::nullopt);
            });

    QObject::connect(_view, &CalculationModesView::selection_changed,
                     this, [this](std::optional<QUuid> modeId) {
                         emit modeSelected(modeId);
                     });
}

QToolBar* CalculationModesPresenter::buildToolbar(QWidget* parent)
{
    auto* tb = new QToolBar("Számítási módok", parent);

    _status = new RepositoryOverlayWidget<NeedCalculationRegistry>(tb, "ModesOverlay");

    refreshOverlayOnly();
    connectTreeStats();

    QAction* addAct    = tb->addAction("➕ Új mód");
    QAction* removeAct = tb->addAction("🗑️ Törlés");
    QAction* renameAct = tb->addAction("✏️ Átnevezés");

    QObject::connect(addAct, &QAction::triggered, this, [this]() {
        ModeNameDialog dlg(_view, "", {});
        if (dlg.exec() != QDialog::Accepted)
            return;
        const QString modeName = dlg.value();
        emit _view->request_add_mode(_treeManager->currentProductId(), modeName);
    });


    QObject::connect(removeAct, &QAction::triggered, this, [this]() {
        auto id = _view->currentModeId();
        if (id) emit _view->request_remove_mode(*id);
    });

    QObject::connect(renameAct, &QAction::triggered, this, [this]() {
        auto id = _view->currentModeId();
        if (id) emit _view->request_rename_mode(*id);
    });

    connectRegistry();
    return tb;
}


void CalculationModesPresenter::connectTreeStats()
{
    QObject::connect(_treeManager, &ProductTreeManager::treeStatsChanged,
                     this, [this](int, int) {
                         refreshOverlayOnly();
                     });
}

void CalculationModesPresenter::refreshForProduct(const QUuid& productId,
                                                  const QString& name,
                                                  const QString& barcode)
{
    auto rows = _manager->refreshForProduct(productId, name, barcode);
    _view->set_modes(rows);
    refreshOverlayOnly();
    _status->refresh(_view->rowCount());
}

void CalculationModesPresenter::refreshOverlayOnly()
{
    _status->refresh(_view->rowCount());
}

void CalculationModesPresenter::connectRegistry()
{
    NeedCalculationRegistry::instance().subscribeItemsChangedToken(
        [this]() {
            refreshOverlayOnly();
        });

}


