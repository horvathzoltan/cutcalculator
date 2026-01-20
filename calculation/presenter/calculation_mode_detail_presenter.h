#pragma once
#include <QObject>
#include <QToolBar>
#include <QUuid>
#include "calculation/view/calculation_mode_detail_view.h"
#include "calculation/manager/calculation_mode_detail_manager.h"
#include "ui/helpers/repository_overlay_widget.h"

class CalculationModeDetailPresenter : public QObject {
    Q_OBJECT
public:
    explicit CalculationModeDetailPresenter(CalculationModeDetailView* view,
                                            CalculationModeDetailManager* manager,
                                            QObject* parent = nullptr);

    QToolBar* buildToolbar(QWidget* parent);

    void refreshForCalculation(const QUuid& calcId,
                               const QString& modeName);

    void refreshOverlayOnly();

signals:
    void modeSelected(std::optional<QUuid> modeId);

public slots:
    void onModeSelected(std::optional<QUuid> modeId);


private:
    CalculationModeDetailView* _view = nullptr;
    CalculationModeDetailManager* _manager = nullptr;
    RepositoryOverlayWidget<NeedCalculationDetailRegistry>* _status = nullptr;
};
