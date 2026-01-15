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
    _subscriptionId =
        NeedCalculationRegistry::instance().subscribeItemsChanged([this]() {
            this->onRegistryChanged();
        });

}

CalculationModesManager::~CalculationModesManager()
{
    NeedCalculationRegistry::instance().unsubscribeItemsChanged(_subscriptionId);
}


void CalculationModesManager::onRegistryChanged()
{
    if (_currentProductId.isNull())
        return;
    refreshForProduct(_currentProductId, _currentProductName, _currentProductBarcode);
}


/* ============================================================
 * 🧩 🔧 Signal kapcsolatok
 * ============================================================ */
void CalculationModesManager::connectSignals() {

    // connect(_view, &CalculationModesView::current_product_changed, this,
    //         &CalculationModesManager::refreshForProduct);

    /* ------------------------------
     * ➕ Új mód hozzáadása
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_add_mode,
            this, [this](const QUuid& productId) {

                // 🧪 Duplikációs ellenőrzés: productId + name egyedisége
                auto duplicateCheck = [&](const QString& name) {
                    return NeedCalculationRegistry::instance().existsBy(
                        [&](const NeedCalculation& x) {
                            return x.productId == productId && x.name == name;
                        }
                        );
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
                    refreshForProduct(_currentProductId, _currentProductName, _currentProductBarcode);
                }
            });

    /* ------------------------------
     * ❌ Mód törlése
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_remove_mode,
            this, [this](const QUuid& id) {
                if (NeedCalculationRegistry::instance().remove(id)) {
                    zEventINFO("🗑️ Mode removed");
                    refreshForProduct(_currentProductId, _currentProductName, _currentProductBarcode);
                }
            });

    /* ------------------------------
     * ✏️ Mód átnevezése
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_rename_mode,
            this, [this](const QUuid& id) {

                const auto* old = NeedCalculationRegistry::instance().findById(id);
                if (!old)
                    return;

                // Duplikációs ellenőrzés ugyanarra a productId-re
                auto duplicateCheck = [&](const QString& name) {
                    return NeedCalculationRegistry::instance().existsBy(
                        [&](const NeedCalculation& x) {
                            return x.productId == old->productId && x.name == name;
                        }
                        );
                };

                ModeNameDialog dlg(_view, old->name, duplicateCheck);
                if (dlg.exec() != QDialog::Accepted)
                    return;

                QString newName = dlg.value();
                if (newName.isEmpty() || newName == old->name)
                    return;

                NeedCalculation updated = *old;
                updated.name = newName;

                if (NeedCalculationRegistry::instance().update(updated)) {
                    zEventINFO(QString("✏️ Mode renamed: %1 → %2").arg(old->name, newName));
                    refreshForProduct(_currentProductId, _currentProductName, _currentProductBarcode);
                }
            });
}

/* ============================================================
 * 🧩 🎨 Nézet frissítése
 * ============================================================ */
void CalculationModesManager::refreshForProduct(const QUuid& productId,
                                                const QString& productName,
                                                const QString& productBarcode)
{
    _currentProductId = productId;
    _currentProductName = productName;
    _currentProductBarcode = productBarcode;

    auto modes = NeedCalculationRegistry::instance().findAll(
        [&](const NeedCalculation& c) {
            return c.productId == productId;
        }
        );


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
