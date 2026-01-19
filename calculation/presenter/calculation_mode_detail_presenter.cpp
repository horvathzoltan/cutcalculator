#include "calculation/presenter/calculation_mode_detail_presenter.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"

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

}

QToolBar* CalculationModeDetailPresenter::buildToolbar(QWidget* parent)
{
    auto* tb = new QToolBar("Formula műveletek", parent);

    _status = new OverlayIconWidget();
    _status->setBaseEmoji("📄");
    tb->addWidget(_status);
    _view->setStatusWidget(_status);
    _status->setObjectName("DetailsOverlay");

    initialOverlay();

    QAction* addAct    = tb->addAction("➕ Új formula");
    QAction* removeAct = tb->addAction("🗑️ Törlés");
    QAction* editAct   = tb->addAction("✏️ Szerkesztés");

    QObject::connect(addAct, &QAction::triggered, this, [this]() {
        emit _view->request_add_detail(_view->currentCalculationId());
    });

    QObject::connect(removeAct, &QAction::triggered, this, [this]() {
        auto tables = _view->findChildren<QTableWidget*>();
        if (tables.isEmpty()) return;
        auto* t = tables.first();
        auto r = t->selectedRanges();
        if (r.isEmpty()) return;
        auto* item = t->item(r.first().topRow(), 0);
        if (!item) return;
        emit _view->request_remove_detail(item->data(Qt::UserRole).toUuid());
    });

    QObject::connect(editAct, &QAction::triggered, this, [this]() {
        auto tables = _view->findChildren<QTableWidget*>();
        if (tables.isEmpty()) return;
        auto* t = tables.first();
        auto r = t->selectedRanges();
        if (r.isEmpty()) return;
        auto* item = t->item(r.first().topRow(), 0);
        if (!item) return;
        emit _view->request_edit_formula(item->data(Qt::UserRole).toUuid());


    });


    return tb;
}

void CalculationModeDetailPresenter::initialOverlay()
{
    int repo = NeedCalculationDetailRegistry::instance().size();
    _status->updateOverlayState2(repo, 0);
}

void CalculationModeDetailPresenter::refreshForCalculation(const QUuid& calcId,
                                                           const QString& modeName)
{
    auto rows = _manager->refreshForCalculation(calcId, modeName);
    _view->set_details(rows);
    // int repo = NeedCalculationDetailRegistry::instance().size();
    // int visible = rows.size();
    // _status->updateOverlayState2(repo, visible);
    refreshOverlayOnly();
}

void CalculationModeDetailPresenter::onModeSelected(std::optional<QUuid> modeId)
{
    if (!modeId) {
        _view->set_details({});
        return;
    }
    const NeedCalculation* mode =
        NeedCalculationRegistry::instance().findById(*modeId);
    QString modeName = mode ? mode->name : QString("unknown");
    refreshForCalculation(*modeId, modeName);
}

void CalculationModeDetailPresenter::refreshOverlayOnly()
{
    int repo = NeedCalculationDetailRegistry::instance().size();
    int visible = _view->rowCount();
    _status->updateOverlayState2(repo, visible);
}
