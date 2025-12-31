#include "calculation/manager/calculation_mode_detail_manager.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "common/registry/registry_manager.h"
#include "common/logger/event_logger.h"
#include "materials/registry/material_registry.h"

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
    connect(_view, &CalculationModeDetailView::request_add_detail, this, [this](const QUuid& calcId) {
        // Placeholder: itt majd MaterialPickerDialog + FormulaEditorDialog lesz.
        NeedCalculationDetail d;
        d.id = QUuid::createUuid();
        d.needCalculationId = calcId;
        d.materialId = QUuid::createUuid();   // szándékosan invalid, hogy jelezze a hiányzó picker szükségességét
        d.formula = "w-0";                    // demo formula

        if (!NeedCalculationDetailRegistry::instance().insert(d)) {
            zWarning("⚠️ Detail insert failed – material or formula invalid");
            return;
        }
        zEventINFO(QString("➕ Detail added for calcId=%1").arg(calcId.toString()));
        // A refresh triggerelése a felhasználói élmény kedvéért:
        refreshForCalculation(calcId, "mode");
    });

    connect(_view, &CalculationModeDetailView::request_remove_detail, this, [](const QUuid& detailId) {
        if (!NeedCalculationDetailRegistry::instance().remove(detailId)) {
            zWarning(QString("⚠️ Detail remove failed: %1").arg(detailId.toString()));
        } else {
            zEventINFO(QString("🗑️ Detail removed: %1").arg(detailId.toString()));
        }
    });

    connect(_view, &CalculationModeDetailView::request_edit_formula, this, [](const QUuid& detailId) {
        // Placeholder: itt majd formula editor dialog lesz.
        const QString newFormula = "fixed:2";
        if (!NeedCalculationDetailRegistry::instance().updateFormula(detailId, newFormula)) {
            zWarning(QString("⚠️ Formula update failed for detailId=%1").arg(detailId.toString()));
        } else {
            zEventINFO(QString("✏️ Formula updated: %1 → %2").arg(detailId.toString(), newFormula));
        }
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
void CalculationModeDetailManager::refreshForCalculation(const QUuid& calcId, const QString& modeName) {
    auto details = NeedCalculationDetailRegistry::instance().findByCalculation(calcId);
    auto rows = makeRows(details);

    _view->set_current_calculation(calcId, modeName);
    _view->set_details(rows);

    zInfo(QString("🔄 Details refreshed for mode: %1, count=%2").arg(modeName).arg(rows.size()));
}

/**
 * @brief Material név és barcode feloldása. Ha nem található, jelöljük ismeretlenként.
 */
std::pair<QString, QString> CalculationModeDetailManager::materialLabel(const QUuid& materialId) {
    auto* repo = RegistryManager::instance().findByTypeName("MaterialMaster");
    if (!repo)
        return { "(unknown)", "" };

    if (const auto* ent = repo->findEntityById(materialId)) {
        return { ent->name, ent->barcode };
    }

    return { "(unknown)", "" };
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
CalculationModeDetailManager::makeRows(const QVector<NeedCalculationDetail>& details) {
    QVector<CalculationModeDetailView::DetailRow> rows;
    rows.reserve(details.size());

    for (const auto& d : details) {
        auto [name, barcode] = materialLabel(d.materialId);
        const bool materialOk = !name.isEmpty() && name != "(unknown)";
        const bool isCut = !d.formula.startsWith("fixed:");
        const bool formulaValid = materialOk; // formula parse-t a registry már ellenőrzi; itt material hiányt jelzünk

        rows.push_back({
            d.id,
            d.needCalculationId,
            d.materialId,
            name,
            barcode,
            d.formula,
            isCut,
            formulaValid
        });
    }
    return rows;
}
