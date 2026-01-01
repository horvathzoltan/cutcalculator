#pragma once
#include <QObject>
#include <QUuid>
#include <QVector>
#include "calculation/view/calculation_mode_detail_view.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/manager/registry_manager.h"
#include "common/logger/event_logger.h"

/**
 * @class CalculationModeDetailManager
 * @brief Glue: NeedCalculation kiválasztás → NeedCalculationDetail lista betöltése és műveletek kezelése.
 *
 * Felelősség:
 * - A kiválasztott számítási mód (calcId) alapján betölti a detail sorokat a registryből.
 * - Material lookup: név és barcode feloldása az IdentifiableRegistryBase segítségével.
 * - A view által kibocsátott jelekre (add/remove/edit) reagál, és CRUD-ot végez a registryben.
 *
 * Integráció:
 * - BOMWorkbench jobb paneljén a CalculationModeDetailView-hoz tartozó manager.
 * - A CalculationModesView selection_changed jelére hívjuk a refreshForCalculation(calcId, modeName) metódust.
 */
class CalculationModeDetailManager : public QObject {
    Q_OBJECT
public:
    explicit CalculationModeDetailManager(CalculationModeDetailView* view, QObject* parent = nullptr);

    /**
     * @brief Detail sorok frissítése egy számítási módhoz tartozóan.
     * @param calcId A NeedCalculation azonosítója (UUID)
     * @param modeName A mód neve (auditbarát loghoz és tooltipekhez)
     */
    void refreshForCalculation(const QUuid& calcId, const QString& modeName);

private:
    CalculationModeDetailView* _view = nullptr;

    void connectSignals();

    // Segéd: material lookup az azonosító alapján
    static std::pair<QString, QString> materialLabel(const QUuid& materialId);

    // Segéd: UI listaépítés a registry rekordokból
    static QVector<CalculationModeDetailView::DetailRow>
    makeRows(const QVector<NeedCalculationDetail>& details);
};
