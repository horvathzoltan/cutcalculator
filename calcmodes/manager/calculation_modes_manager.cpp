#include "calcmodes/manager/calculation_modes_manager.h"
#include "common/logger/event_logger.h"
#include "calcmodes/dialogs/mode_name_dialog.h"

CalculationModesManager::CalculationModesManager(CalculationModesView* view, QObject* parent)
    : QObject(parent), _view(view)
{
    connectSignals();
    reloadAll(); // initial load from CSV
}

void CalculationModesManager::connectSignals() {
    connect(_view, &CalculationModesView::request_add_mode, this, [this](const QUuid& productId) {
        NeedCalculation c;
        c.id = QUuid::createUuid();
        c.productDefinitionId = productId;
        connect(_view, &CalculationModesView::request_add_mode, this, [this](const QUuid& productId) {
            ModeNameDialog dlg;
            if (dlg.exec() != QDialog::Accepted)
                return;

            const QString name = dlg.modeName();
            if (name.isEmpty())
                return; // extra védőháló, bár az OK gomb üresen nem engedélyezett

            NeedCalculation c;
            c.id = QUuid::createUuid();
            c.productDefinitionId = productId;
            c.modeName = name;

            if (NeedCalculationRegistry::instance().insert(c)) {
                reloadAll();
            }
        });
        // Hunglish default – később dialogból
        if (NeedCalculationRegistry::instance().insert(c)) {
            reloadAll();
        }
    });

    connect(_view, &CalculationModesView::request_remove_mode, this, [this](const QUuid& id) {
        if (NeedCalculationRegistry::instance().remove(id)) {
            reloadAll();
        }
    });

    connect(_view, &CalculationModesView::request_rename_mode, this, [this](const QUuid& id) {
        // Hunglish: no dialog here; placeholder rename
        NeedCalculationRegistry::instance().rename(id, "glass");
        reloadAll();
    });
}

void CalculationModesManager::reloadAll() {
    // Load CSVs only once at startup ideally; here safe for now
    QVector<NeedCalculation> calcs;
    NeedCalculationRepository::load(calcs);
    NeedCalculationRegistry::instance().setAll(calcs);

    QVector<NeedCalculationDetail> details;
    NeedCalculationDetailRepository::load(details);
    NeedCalculationDetailRegistry::instance().setAll(details);
}

void CalculationModesManager::refreshForProduct(const QUuid& productId, const QString& productName, const QString& productBarcode) {
    auto modes = NeedCalculationRegistry::instance().findByProduct(productId);
    QVector<CalculationModesView::ModeRow> rows;
    rows.reserve(modes.size());
    for (const auto& m : modes) {
        auto dets = NeedCalculationDetailRegistry::instance().findByCalculation(m.id);
        //rows.append({ m.id, productId, m.modeName, dets.size() });
        int detailCount = static_cast<int>(dets.size());
        rows.append({ m.id, productId, m.modeName, detailCount });

    }
    _view->set_current_product(productId, productName, productBarcode);
    _view->set_modes(rows);
    zEventINFO(QString("🔄 Modes refreshed: %1 (%2)").arg(productName, productBarcode));
}
