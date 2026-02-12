#include "calculation/presenter/calculation_mode_detail_presenter.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "ui/helpers/repository_overlay_widget.h"

#include <calculation/service/matrix_validator.h>

CalculationModeDetailPresenter::CalculationModeDetailPresenter(
    CalculationModeDetailView* view,
    CalculationModeDetailManager* manager,
    QObject* parent)
    : QObject(parent)
    , _view(view)
    , _manager(manager)
{

    // QObject::connect(_view->parent(), SIGNAL(selection_changed(std::optional<QUuid>)),
    //                  this, [this](std::optional<QUuid> modeId) {
    //                      if (!modeId) {
    //                          _view->set_details({});
    //                          return;
    //                      }
    //                      const NeedCalculation* mode =
    //                          NeedCalculationRegistry::instance().findById(*modeId);
    //                      QString modeName = mode ? mode->name : QString("unknown");
    //                      refreshForCalculation(*modeId, modeName);
    //                  });

     QObject::connect(this, &CalculationModeDetailPresenter::modeSelected,
                      this, &CalculationModeDetailPresenter::onModeSelected);

    connect(this, &CalculationModeDetailPresenter::requestScrollToRow,
            _view, &CalculationModeDetailView::scrollToRow);
}

QToolBar* CalculationModeDetailPresenter::buildToolbar(QWidget* parent)
{
    auto* tb = new QToolBar("Formula műveletek", parent);

    _status = new RepositoryOverlayWidget<NeedCalculationDetailRegistry>(tb, "DetailsOverlay");

    refreshOverlayOnly();

    //QAction* addAct    = tb->addAction("➕ Új formula");
    //QAction* removeAct = tb->addAction("🗑️ Törlés");
    QAction* editAct   = tb->addAction("✏️ Szerkesztés");

    // QObject::connect(addAct, &QAction::triggered, this, [this]() {
    //     emit _view->request_add_detail(_view->currentCalculationId());
    // });

    // QObject::connect(removeAct, &QAction::triggered, this, [this]() {
    //     auto tables = _view->findChildren<QTableWidget*>();
    //     if (tables.isEmpty()) return;
    //     auto* t = tables.first();
    //     auto r = t->selectedRanges();
    //     if (r.isEmpty()) return;
    //     auto* item = t->item(r.first().topRow(), 0);
    //     if (!item) return;
    //     emit _view->request_remove_detail(item->data(Qt::UserRole).toUuid());
    // });

    QObject::connect(editAct, &QAction::triggered, this, [this]() {
        auto tables = _view->findChildren<QTableWidget*>();
        if (tables.isEmpty()) return;
        auto* t = tables.first();
        auto r = t->selectedRanges();
        if (r.isEmpty()) return;
        auto* item = t->item(r.first().topRow(), 0);
        if (!item) return;
        //emit _view->request_edit_formula(item->data(Qt::UserRole).toUuid());
        QUuid id = item->data(Qt::UserRole).toUuid();
        emit _view->request_edit_formula(id);

    });

    connectRegistry();

    return tb;
}


void CalculationModeDetailPresenter::refreshForCalculation(const QUuid& calcId,
                                                           const QString& modeName)
{
    auto rows = _manager->refreshForCalculation(calcId, modeName);

    refreshOverlayOnly();

    int firstProblem = -1;
    for (int i = 0; i < rows.size(); ++i) {
        const auto& r = rows[i];
        if (!r.materialValid || !r.formulaValid || r.formula.trimmed().isEmpty()) {
            firstProblem = i;
            break;
        }
    }
    emit requestScrollToRow(firstProblem >= 0 ? firstProblem : 0);
}

void CalculationModeDetailPresenter::onModeSelected(std::optional<QUuid> modeId)
{
    if (!modeId) {
        return;
    }
    const NeedCalculation* mode =
        NeedCalculationRegistry::instance().findById(*modeId);
    QString modeName = mode ? mode->name : QString("unknown");

    _view->set_current_calculation(*modeId, modeName);

    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(*modeId);
    if (!details.isEmpty()) {
        refreshForCalculation(*modeId, mode->name);
    }
}

void CalculationModeDetailPresenter::refreshOverlayOnly()
{
    _status->refresh(_view->rowCount());
}

void CalculationModeDetailPresenter::connectRegistry()
{
    NeedCalculationDetailRegistry::instance().subscribeItemsChangedToken(
        [this]() {
            QUuid calcId = _view->currentCalculationId();
            if (calcId.isNull()) {
                _view->set_details({});
                refreshOverlayOnly();
                return;
            }

            const NeedCalculation* mode =
                NeedCalculationRegistry::instance().findById(calcId);
            QString modeName = mode ? mode->name : QString("unknown");

            // újratöltjük a sorokat
            auto rows = _manager->refreshForCalculation(calcId, modeName);

            // scroll a hibás sorra
            int firstProblem = -1;
            for (int i = 0; i < rows.size(); ++i) {
                const auto& r = rows[i];
                if (!r.materialValid || !r.formulaValid) {
                    firstProblem = i;
                    break;
                }
            }
            emit requestScrollToRow(firstProblem >= 0 ? firstProblem : 0);

            refreshOverlayOnly();
        });

}

OverlayStatusHelper::State CalculationModeDetailPresenter::computeMatrixState()
{
    // v2: Overlay NEM jelzi a mátrixot, csak repoCount + visibleRows
    int repo = NeedCalculationDetailRegistry::instance().size();
    int visible = _view->rowCount();

    if (repo == 0)
        return OverlayStatusHelper::State::EmptyRepo;

    if (visible == 0)
        return OverlayStatusHelper::State::NoVisibleRows;

    return OverlayStatusHelper::State::Normal;
}


