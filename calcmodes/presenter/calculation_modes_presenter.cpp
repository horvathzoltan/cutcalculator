#include "calcmodes/presenter/calculation_modes_presenter.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/dialogs/mode_name_dialog.h"

#include <ui/helpers/repository_overlay_widget.h>
#include <ui/helpers/overlay_status_helper.h>

#include <needs/registry/need_rule_registry.h>

#include <calculation/service/matrix_generator.h>

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
                // v2: ha a mód hiányos, a detail nézet automatikusan a hibára ugrik
            });

    QObject::connect(_view, &CalculationModesView::selection_changed,
                     this, [this](std::optional<QUuid> modeId) {
                         emit modeSelected(modeId);
                         if (modeId) on_mode_activated(*modeId);
                     });

    connect(_view, &CalculationModesView::modeActivated,
            this, &CalculationModesPresenter::on_mode_activated);
}

void CalculationModesPresenter::on_mode_activated(const QUuid& modeId)
{
    // 🛠 1) Hiányzó detail cellák felmérése ehhez a módhoz
    auto missing = MatrixValidator::validateMode(modeId);

    if (!missing.isEmpty()) {
        zInfo(QString("🛠 MatrixGenerator: generating %1 missing details for mode %2")
                  .arg(missing.size())
                  .arg(modeId.toString()));

        // 🛠 2) Hiányzó cellák pótlása
        zInfo(QString("🧩 Auto-heal triggered for mode %1").arg(modeId.toString()));
        zInfo("Reason: missing detail cells detected");

        MatrixGenerator::generate(missing);

        // 🧾 3) Audit log minden generált celláról
        for (const auto& md : missing) {
            zInfo(QString("GEN detail: product=%1 mode=%2 material=%3")
                      .arg(md.productId.toString())
                      .arg(md.modeId.toString())
                      .arg(md.materialId.toString()));
        }

        // 🟢 4) GUI frissítése generálás után
        auto* mode = NeedCalculationRegistry::instance().findById(modeId);
        refreshForProduct(mode->productId, QString(), QString());
    }

    // 🔁 4) Detail view frissítése (eddigi logika)
    emit modeActivatedForDetails(modeId);
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


OverlayStatusHelper::State CalculationModesPresenter::computeMatrixState()
{
    int repo = NeedCalculationRegistry::instance().size();
    int visible = _view->rowCount();

    if (repo == 0)
        return OverlayStatusHelper::State::EmptyRepo;

    if (visible == 0)
        return OverlayStatusHelper::State::NoVisibleRows;

    // v2: Overlay NEM jelzi a mátrixot

    return OverlayStatusHelper::State::Normal;

}

// bool CalculationModesPresenter::isMatrixComplete() const
// {
//     const auto modes = NeedCalculationRegistry::instance().readAll();
//     const auto rules = NeedRuleRegistry::instance().readAll();

//     for (const auto& m : modes) {
//         for (const auto& r : rules) {
//             if (r.leftId != m.productId)
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
