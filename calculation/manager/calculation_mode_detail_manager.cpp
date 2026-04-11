#include "calculation/manager/calculation_mode_detail_manager.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/manager/registry_manager.h"

#include "dsl/formula_analysis.h"
#include "dsl/formula_contract.h"
#include "materials/registry/material_registry.h"
#include "materials/model/cutting_mode.h"

CalculationModeDetailManager::CalculationModeDetailManager(CalculationModeDetailView* view, QObject* parent)
    : QObject(parent), _view(view)
{
    connectSignals();
}


void CalculationModeDetailManager::connectSignals() {

    connect(_view, &CalculationModeDetailView::request_update_formula,
            this, [this](const QUuid& detailId, const QString& newFormula) {

                // 1) Workflow-alapú update
                if (!NeedCalculationDetailRegistry::instance()
                         .updateFormula(detailId, newFormula)) {

                    zEventWARN(QString("⚠️ Formula update failed (id=%1)")
                                   .arg(detailId.toString()));
                    return;
                }

                // 2) A presenter automatikusan frissít a notifyItemsChanged miatt
            });

}

/**
 * @brief Detail sorok frissítése egy adott számítási módhoz.
 *
 * Lépések:
 * - Lekérjük a registryből az adott calcId-hoz tartozó NeedCalculationDetail sorokat.
 * - Material lookup: név + barcode feloldása az IdentifiableRegistryBase segítségével.
 * - Heurisztika: fixed: prefix → kitting (📦), különben cutting (⚙️).
 * - A kész sorokat átadjuk a view-nak.
 */
QVector<CalculationModeDetailView::DetailRow> CalculationModeDetailManager::refreshForCalculation(const QUuid& calcId,
                                                                                                  const QString& modeName) {
    // v2: Manager nem végez mátrix-validációt


    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    auto rows = makeRows(details); // v2: mátrix-információ nem része a soroknak

    // 🔧 ÚJ: a view tudja, melyik calcId aktív
    _view->set_current_calculation(calcId, modeName);

    // 🔄 Detail sorok átadása a view-nak
    _view->set_details(rows);

    return rows;
}


/**
 * @brief Material név és barcode feloldása. Ha nem található, jelöljük ismeretlenként.
 */
std::pair<QString, QString> CalculationModeDetailManager::materialLabel(const QUuid& materialId) {
    const auto* entBase =
        RegistryManager::instance().findEntity("MaterialMaster", materialId);
    if (!entBase)
        return { "(unknown)", "" };

    const auto* ent = dynamic_cast<const BarcodeIdentifiableEntity*>(entBase);
    if (!ent)
        return { "(unknown)", "" };

    return { ent->name, ent->barcode };
}


/**
 * @brief NeedCalculationDetail → DetailRow konverzió.
 *
 * Heurisztika:
 * - fixed:<n> → kitting (isCutting=false)
 * - w-/h- kezdetű → cutting (isCutting=true)
 * - Material hiány esetén formulaValid=false → piros háttér a view-ban.
 */
QVector<CalculationModeDetailView::DetailRow>
CalculationModeDetailManager::makeRows(const QVector<NeedCalculationDetail>& details)
{
    QVector<CalculationModeDetailView::DetailRow> rows;
    rows.reserve(details.size());

    for (const auto& d : details) {
        auto [name, barcode] = materialLabel(d.materialId);
        const bool materialOk = !name.isEmpty() && name != "(unknown)";

        bool isCutting = d.kind == NeedCalculationDetail::DetailKind::Cutting;
        FormulaContract c = isCutting ? cuttingContract() : kittingContract();

        const bool formulaValid = analyzeFormula(d.formula, c).ok;
        const auto* mat = MaterialRegistry::instance().findById(d.materialId);
        CuttingMode mode = mat ? mat->cuttingMode : CuttingMode::Unknown;

        rows.push_back({
            d.id,
            d.needCalculationId,
            d.materialId,
            name,
            barcode,
            d.formula,
            formulaValid,
            materialOk,
            mode,
            d.kind
        });


    }
    return rows;
}


void CalculationModeDetailManager::resetView(){
    _view->reset();
}
