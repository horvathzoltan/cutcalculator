#include "calcmodes/manager/calculation_modes_manager.h"
#include "common/logger/event_logger.h"
#include "calcmodes/dialogs/mode_name_dialog.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/repository/need_calculation_repository.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "calculation/repository/need_calculation_detail_repository.h"

/* ============================================================
 * 🧩 Konstruktor
 * ============================================================ */
CalculationModesManager::CalculationModesManager(CalculationModesView* view, QObject* parent)
    : QObject(parent), _view(view)
{
    connectSignals();
    reloadAll();
}

/* ============================================================
 * 🧩 🔧 Signal kapcsolatok
 * ============================================================ */
void CalculationModesManager::connectSignals() {

    /* ------------------------------
     * ➕ Új mód hozzáadása
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_add_mode,
            this, [this](const QUuid& productId) {

                // 🧪 Duplikációs ellenőrzés callback
                auto duplicateCheck = [&](const QString& name) {
                    return NeedCalculationRegistry::instance().exists(productId, name);
                };

                // 🎨 Dialógus megnyitása
                ModeNameDialog dlg(_view, "", duplicateCheck);
                if (dlg.exec() != QDialog::Accepted)
                    return;

                const QString name = dlg.value();
                if (name.isEmpty())
                    return;

                // 📦 Új entitás
                NeedCalculation c;
                c.id = QUuid::createUuid();
                c.productId = productId;
                c.name = name;

                if (NeedCalculationRegistry::instance().insert(c)) {
                    zEventINFO(QString("➕ New mode added: %1").arg(name));
                    reloadAll();
                }
            });

    /* ------------------------------
     * ❌ Mód törlése
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_remove_mode,
            this, [this](const QUuid& id) {
                if (NeedCalculationRegistry::instance().remove(id)) {
                    zEventINFO("🗑️ Mode removed");
                    reloadAll();
                }
            });

    /* ------------------------------
     * ✏️ Mód átnevezése
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_rename_mode,
            this, [this](const QUuid& id) {

                auto old = NeedCalculationRegistry::instance().findById(id);
                if (!old)
                    return;

                auto duplicateCheck = [&](const QString& name) {
                    return NeedCalculationRegistry::instance().exists(old->productId, name);
                };

                ModeNameDialog dlg(_view, old->name, duplicateCheck);
                if (dlg.exec() != QDialog::Accepted)
                    return;

                QString newName = dlg.value();
                if (newName.isEmpty())
                    return;

                if (NeedCalculationRegistry::instance().rename(id, newName)) {
                    zEventINFO(QString("✏️ Mode renamed: %1 → %2").arg(old->name, newName));
                    reloadAll();
                }
            });
}

/* ============================================================
 * 🧩 📦 Adatok újratöltése
 * ============================================================ */
void CalculationModesManager::reloadAll() {
    QVector<NeedCalculation> calcs;
    NeedCalculationRepository::load(calcs);
    NeedCalculationRegistry::instance().setAll(calcs);

    QVector<NeedCalculationDetail> details;
    NeedCalculationDetailRepository::load(details);
    NeedCalculationDetailRegistry::instance().setAll(details);
}

/* ============================================================
 * 🧩 🎨 Nézet frissítése
 * ============================================================ */
void CalculationModesManager::refreshForProduct(const QUuid& productId,
                                                const QString& productName,
                                                const QString& productBarcode)
{
    auto modes = NeedCalculationRegistry::instance().findByProduct(productId);

    QVector<CalculationModesView::ModeRow> rows;
    rows.reserve(modes.size());

    for (const auto& m : modes) {
        int detailCount =
            NeedCalculationDetailRegistry::instance().findByCalculation(m.id).size();

        rows.append({ m.id, productId, m.name, detailCount });
    }

    _view->set_current_product(productId, productName, productBarcode);
    _view->set_modes(rows);

    zEventINFO(QString("🔄 Modes refreshed: %1 (%2)").arg(productName, productBarcode));
}
