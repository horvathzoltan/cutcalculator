#include "calculation/manager/calculation_mode_detail_manager.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/manager/registry_manager.h"
#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/dialogs/formula_editor_dialog.h"
#include "calculation/service/matrix_validator.h"

CalculationModeDetailManager::CalculationModeDetailManager(CalculationModeDetailView* view, QObject* parent)
    : QObject(parent), _view(view)
{
    connectSignals();
}

/**
 * A view jeleinek bekötése:
 * - request_add_detail(calcId): új sor felvétele (material + formula) – itt egyelőre placeholder.
 * - request_remove_detail(detailId): törlés a registryben.
 * - request_edit_formula(detailId): formula frissítése – egyszerű demo érték.
 *
 * A valódi workflow-hoz anyagválasztó és formula szerkesztő dialogokat érdemes beépíteni.
 */
void CalculationModeDetailManager::connectSignals() {

    connect(_view, &CalculationModeDetailView::request_edit_formula,
            this, [this](const QUuid& detailId) {

                const auto* d = NeedCalculationDetailRegistry::instance().findById(detailId);
                if (!d) return;

                FormulaEditorDialog dlg(d->formula, _view);
                if (dlg.exec() != QDialog::Accepted)
                    return;

                auto updated = *d;
                updated.formula = dlg.formula();

                NeedCalculationDetailRegistry::instance().update(updated);
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
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);

    if (details.isEmpty()) {
        const auto rules =
            NeedRuleRegistry::instance().findByLeft(
                NeedCalculationRegistry::instance().findById(calcId)->productId);

        for (const auto& r : rules) {
            NeedCalculationDetail d;
            d.id = QUuid::createUuid();
            d.needCalculationId = calcId;
            d.materialId = r.rightId;
            d.formula = "w-0";
            d.kind = NeedCalculationDetail::DetailKind::Cutting;
            NeedCalculationDetailRegistry::instance().insertWithWorkflow(d);
        }

        details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);
    }

    bool matrixComplete = MatrixValidator::isCalculationMatrixComplete(calcId);
    auto rows = makeRows(details, matrixComplete);

    // 🔧 ÚJ: a view tudja, melyik calcId aktív
    _view->set_current_calculation(calcId, modeName);

    // 🔄 Detail sorok átadása a view-nak
    _view->set_details(rows);

    // int repoCount = NeedCalculationDetailRegistry::instance().size();
    // int visibleRows = rows.size();
    // _view->updateOverlay(repoCount, visibleRows);

    // zInfo(QString("🔄 Details refreshed for mode: %1, count=%2").arg(modeName).arg(rows.size()));

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
CalculationModeDetailManager::makeRows(const QVector<NeedCalculationDetail>& details, bool matrixComplete) {
    QVector<CalculationModeDetailView::DetailRow> rows;
    rows.reserve(details.size());

    for (const auto& d : details) {
        auto [name, barcode] = materialLabel(d.materialId);
        const bool materialOk = !name.isEmpty() && name != "(unknown)";
        const bool isCut = (d.kind == NeedCalculationDetail::DetailKind::Cutting);
        const bool formulaValid = materialOk; // formula parse-t a registry már ellenőrzi; itt material hiányt jelzünk

        rows.push_back({
            d.id,
            d.needCalculationId,
            d.materialId,
            name,
            barcode,
            d.formula,
            isCut,
            formulaValid,
            materialOk,
            matrixComplete
        });
    }
    return rows;
}

void CalculationModeDetailManager::resetView(){
    _view->reset();
}
