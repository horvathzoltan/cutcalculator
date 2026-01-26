#include "needs/manager/material_requirements_manager.h"
//#include "materials/registry/material_registry.h"
#include "needs/repository/need_rule_repository.h"
//#include "needs/repository/need_rule_traits.h"
#include "needs/model/need_rule.h"
#include "needs/view/material_picker_dialog.h"
#include "common/logger/event_logger.h"

#include <calculation/service/matrix_validator.h>

MaterialRequirementsManager::MaterialRequirementsManager(MaterialRequirementsView* view, QObject* parent)
    : QObject(parent), _view(view)
{
    connectSignals();

    _subscriptionId =
        NeedRuleRegistry::instance().subscribeItemsChanged([this]() {
            if (!_currentProductId.isNull())
                refreshForProduct(_currentProductId, _currentProductName, _currentProductBarcode);
        });

}

void MaterialRequirementsManager::connectSignals() {
    // Add: a view jelzi, hogy szeretne új kapcsolatot hozzáadni az aktuális producthoz
    connect(_view, &MaterialRequirementsView::request_add_requirement,
             this, [this](const QUuid& productId, const QUuid& materialId) {

                //Q_UNUSED(productBarcode);

                // 🔍 MaterialPickerDialog – user választ anyagot
                // MaterialPickerDialog dlg(_view);
                // if (dlg.exec() == QDialog::Accepted) {
                //     auto result = dlg.result();

                //     NeedRule rule;
                //     rule.leftId = productId;
                //     rule.rightId = result.material_id;

                //     // Registry insert
                //     NeedRuleRegistry::instance().insert(rule);

                //     // Audit log
                //     zEventINFO(QString("➕ NeedRule added: Product=%1 Material=%2")
                //                    .arg(productId.toString(), result.material_id.toString()));

                //     // CSV persist
                //     NeedRuleRepository::save();
                // }
                NeedRule rule;
                rule.leftId = productId;
                rule.rightId = materialId;

                if (!NeedRuleRegistry::instance().insert(rule)) {
                    zWarning("⚠️ NeedRule insert failed");
                    return;
                }

                /*NeedRuleRepository::save();
                zEventINFO(QString("➕ NeedRule added: Product=%1 Material=%2")
                               .arg(productId.toString(), materialId.toString()));*/


            });

    // Remove: a view jelzi a kiválasztott kapcsolat törlését
    connect(_view, &MaterialRequirementsView::request_remove_requirement,
            this, [this](const QUuid& productId,
                   const QString& productBarcode,
                   const QUuid& materialId,
                   const QString& materialBarcode) {
                Q_UNUSED(productBarcode);
                Q_UNUSED(materialBarcode);

                bool ok = NeedRuleRegistry::instance().remove(productId, materialId);
                if (ok) {
                    // zEventINFO(QString("🗑 NeedRule removed: Product=%1 Material=%2")
                    //                .arg(productId.toString(), materialId.toString()));

                    // CSV persist
                    NeedRuleRepository::save();
                } else {
                    zWarning(QString("⚠️ NeedRule remove failed: Product=%1 Material=%2")
                                   .arg(productId.toString(), materialId.toString()));
                }
            });

    connect(_view, &MaterialRequirementsView::request_change_material,
            this, [this](const QUuid& ruleId) {

                const auto* r = NeedRuleRegistry::instance().findById(ruleId);
                if (!r) return;

                MaterialPickerDialog dlg(_view);
                if (dlg.exec() != QDialog::Accepted)
                    return;

                auto updated = *r;
                updated.rightId = dlg.selectedMaterial();

                NeedRuleRegistry::instance().update(updated);
            });

}

QVector<MaterialRequirementsView::RequirementRow>
MaterialRequirementsManager::makeRowsForProduct(const QUuid& productId,
                                                const QString& productName,
                                                const QString& productBarcode)
{
    QVector<MaterialRequirementsView::RequirementRow> out;
    const auto rulesByProduct = NeedRuleRegistry::instance().findByLeft(productId);

    for (const auto& rule : rulesByProduct) {
        MaterialRequirementsView::RequirementRow r;
        r.product_id = productId;
        r.product_name = productName;
        r.product_barcode = productBarcode;

        r.material_id = rule.rightId;

        // Lookup Material

        auto matOpt = rule.right();

        if (matOpt.has_value()){
            const auto& mat = *matOpt;
            r.material_name = mat.name;
            r.material_barcode = mat.barcode;
            r.matrixComplete = MatrixValidator::isProductMatrixComplete(productId);
        } else {
            r.material_name = "UNKNOWN";
            r.material_barcode = "";
            r.matrixComplete = MatrixValidator::isProductMatrixComplete(productId);
        }

        out.append(r);
    }

    return out;
}

QVector<MaterialRequirementsView::RequirementRow> MaterialRequirementsManager::refreshForProduct(const QUuid& productId,
                                                    const QString& productName,
                                                    const QString& productBarcode)
{
    _currentProductId = productId;
    _currentProductName = productName;
    _currentProductBarcode = productBarcode;

    QVector<MaterialRequirementsView::RequirementRow> rows =
        makeRowsForProduct(productId, productName, productBarcode);

    //_view->set_current_product(productId, productName, productBarcode);
    //_view->set_requirements(rows);

    //int repoCount = NeedRuleRegistry::instance().size();
    //int visibleRows = rows.size();
    //_view->updateOverlay(repoCount, visibleRows);

    return rows;

}


