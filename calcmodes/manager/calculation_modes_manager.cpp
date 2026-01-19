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

    /* ------------------------------
     * ➕ Új mód hozzáadása
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_add_mode,
            this, [this](const QUuid& productId, const QString& modeName) {


                // 🧪 Duplikációs ellenőrzés: productId + name egyedisége
                auto duplicateCheck = [&](const QString& name) {
                    return NeedCalculationRegistry::instance().existsBy(
                        [&](const NeedCalculation& x) {
                            return x.productId == productId && x.name == name;
                        }
                        );
                };

                // 🎨 Dialógus megnyitása
                // ModeNameDialog dlg(_view, "", duplicateCheck);
                // if (dlg.exec() != QDialog::Accepted)
                //     return;

                 //const QString name = dlg.value();
                // if (name.isEmpty())
                //     return;

                // // 📦 Új entitás
                // NeedCalculation c;
                // c.id = QUuid::createUuid();
                // c.productId = productId;
                // c.name = name;

                // if (NeedCalculationRegistry::instance().insert(c)) {
                //     zEventINFO(QString("➕ New mode added: %1").arg(name));
                // }

                NeedCalculation nc;
                nc.id = QUuid::createUuid();
                nc.productId = productId;
                nc.name = modeName;
                //NeedCalculationRegistry::instance().insert(nc);
                //NeedCalculationRepository::save();
                if (NeedCalculationRegistry::instance().insert(nc)) {
                     zEventINFO(QString("➕ New mode added: %1").arg(modeName));
                }

            });

    /* ------------------------------
     * ❌ Mód törlése
     * ------------------------------ */
    connect(_view, &CalculationModesView::request_remove_mode,
            this, [this](const QUuid& id) {
                if (NeedCalculationRegistry::instance().remove(id)) {
                    zEventINFO("🗑️ Mode removed");
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
                }
            });
}

/* ============================================================
 * 🧩 🎨 Nézet frissítése
 * ============================================================ */
QVector<CalculationModesView::ModeRow> CalculationModesManager::refreshForProduct(const QUuid& productId,
                                                const QString& productName,
                                                const QString& productBarcode)
{
    _currentProductId = productId;
    _currentProductName = productName;
    _currentProductBarcode = productBarcode;

    QHash<QUuid,int> detailCountByCalc;
    auto allDetails = NeedCalculationDetailRegistry::instance().findAll(
        [](const NeedCalculationDetail&) { return true; }
        );
    for (const auto& d : allDetails) {
        ++detailCountByCalc[d.needCalculationId];
    }

    auto modes = NeedCalculationRegistry::instance().findAll(
        [&](const NeedCalculation& c) {
            return c.productId == productId;
        }
        );

    // auto modes = NeedCalculationRegistry::instance().findAll(
    //     [&](const NeedCalculation& c) {
    //         return c.productId == productId;
    //     }
    //     );


    QVector<CalculationModesView::ModeRow> rows;
    rows.reserve(modes.size());

    for (const auto& m : modes) {
        int detailCount = detailCountByCalc.value(m.id, 0);
        rows.append({ m.id, productId, m.name, detailCount });
    }


    _view->set_current_product(productId, productName, productBarcode);
    // _view->set_modes(rows);

    int repoCount = NeedCalculationRegistry::instance().size();
    int visibleRows = rows.size();
    _view->updateOverlay(repoCount, visibleRows);

    // zEventINFO(QString("🔄 Modes refreshed: %1 (%2)").arg(productName, productBarcode));

    zEventINFO(QString("🔄 Modes refreshed: %1 (%2)").arg(productName, productBarcode));
    return rows;
}


