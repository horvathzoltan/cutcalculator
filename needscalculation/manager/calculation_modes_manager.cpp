#include "needscalculation/manager/calculation_modes_manager.h"
#include "common/logger/event_logger.h"

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
        c.modeName = "falc"; // Hunglish default – később dialogból
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
        rows.append({ m.id, productId, m.modeName, dets.size() });
    }
    _view->set_current_product(productId, productName, productBarcode);
    _view->set_modes(rows);
    zEventINFO(QString("🔄 Modes refreshed: %1 (%2)").arg(productName, productBarcode));
}
