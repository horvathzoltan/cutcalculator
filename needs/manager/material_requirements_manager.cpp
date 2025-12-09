#include "needs/manager/material_requirements_manager.h"
#include "materials/registry/material_registry.h" // feltételezve, hogy van
#include "common/logger/event_logger.h"

MaterialRequirementsManager::MaterialRequirementsManager(MaterialRequirementsView* view, QObject* parent)
    : QObject(parent), _view(view)
{
    connectSignals();
}

void MaterialRequirementsManager::connectSignals() {
    // Add: a view jelzi, hogy szeretne új kapcsolatot hozzáadni az aktuális producthoz
    connect(_view, &MaterialRequirementsView::request_add_requirement,
            this, [this](const QUuid& productId, const QString& productBarcode) {
                Q_UNUSED(productBarcode);

                // Hunglish: itt tipikusan MaterialPickerDialog-ot hívnánk,
                // de mivel a managernek nincs UI felelőssége, csak példa:
                // A tényleges anyagválasztás BOMWorkbench-ben történt (bal oldali fa + jobb oldali picker).
                // Itt kezelhetnénk egy "deferred" add-ot is, ha van dependency.

                zEventINFO(QString("➕ Add requested for productId=%1").arg(productId.toString()));
                // Nincs azonnali hozzáadás itt – BOMWorkbench connect végzi el a sor hozzáadását a view-ban.
            });

    // Remove: a view jelzi a kiválasztott kapcsolat törlését
    connect(_view, &MaterialRequirementsView::request_remove_requirement,
            this, [this](const QUuid& productId,
                   const QString& productBarcode,
                   const QUuid& materialId,
                   const QString& materialBarcode) {
                Q_UNUSED(productBarcode);
                Q_UNUSED(materialBarcode);

                NeedRuleRegistry::instance().remove(productId, materialId);
                // Refresh az aktuális productra (ha van)
                refreshForProduct(productId, QString(), QString());
            });
}

void MaterialRequirementsManager::refreshForProduct(const QUuid& productId,
                                                    const QString& productName,
                                                    const QString& productBarcode)
{
    auto rows = makeRowsForProduct(productId, productName, productBarcode);
    _view->set_requirements(rows);
}

QVector<MaterialRequirementsView::RequirementRow>
MaterialRequirementsManager::makeRowsForProduct(const QUuid& productId,
                                                const QString& productName,
                                                const QString& productBarcode)
{
    QVector<MaterialRequirementsView::RequirementRow> out;
    const auto rules = NeedRuleRegistry::instance().findByProduct(productId);

    // Hunglish: Material adatok lookup-hoz feltételezzük, hogy van MaterialRegistry
    // Ha nincs, akkor material_exists=false és a név/barcode üres.
    for (const auto& rule : rules) {
        MaterialRequirementsView::RequirementRow r;
        r.product_id = productId;
        r.product_name = productName;
        r.product_barcode = productBarcode;

        r.material_id = rule.rightId;

        // Lookup: név + barcode kitöltése registryből, ha elérhető
        // Ha nincs MaterialRegistry, ezt commentben hagyjuk – a warning megjelenik a view-ban.
        // Example (pseudo):
        // if (auto* mat = MaterialRegistry::instance().findById(rule.rightId)) {
        //     r.material_name = mat->name;
        //     r.material_barcode = mat->barcode;
        //     r.material_exists = true;
        // } else {
        //     r.material_name = QString("UNKNOWN");
        //     r.material_barcode = QString();
        //     r.material_exists = false;
        // }

        r.material_exists = !r.material_name.isEmpty(); // ha üres, a view pirosít

        out.append(r);
    }

    return out;
}
