#pragma once
#include <QObject>
#include "needscalculation/view/calculation_modes_view.h"
#include "needscalculation/registry/need_calculation_registry.h"
#include "needscalculation/registry/need_calculation_detail_registry.h"
#include "needscalculation/repository/need_calculation_repository.h"
#include "needscalculation/repository/need_calculation_detail_repository.h"

/**
 * @class CalculationModesManager
 * @brief Glue: product selection → NeedCalculation list, CRUD + persist.
 */
class CalculationModesManager : public QObject {
    Q_OBJECT
public:
    explicit CalculationModesManager(CalculationModesView* view, QObject* parent = nullptr);

    void refreshForProduct(const QUuid& productId, const QString& productName, const QString& productBarcode);

private:
    CalculationModesView* _view = nullptr;

    void connectSignals();
    void reloadAll();
};
