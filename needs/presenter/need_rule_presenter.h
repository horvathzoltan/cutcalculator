#pragma once
#include <QVector>
#include "../registry/need_rule_registry.h"
#include "../repository/need_rule_repository.h"
#include "../../common/logger/event_logger.h"

// Hunglish: tedd a saját view headeredre – itt csak a felhasznált mezőket jelezzük.
#include "../view/material_requirements_view.h"

/**
 * 🎤 NeedRulePresenter – MVP "Presenter"
 *
 * Hunglish: a view passzív, a presenter lookupolja a material törzset, és
 * összerakja a RequirementRow-kat a NeedRule kapcsolat alapján.
 */
class NeedRulePresenter {
    MaterialRequirementsView* _view;

public:
    explicit NeedRulePresenter(MaterialRequirementsView* view)
        : _view(view) {}

    // Hunglish: init – repo load, és a view frissítése egy konkrét Product-ra
    bool initializeAndRefreshForProduct(const QUuid& productId) {
        const bool ok = NeedRuleRepository::load();
        if (!ok) {
            zWarning("⚠️ NeedRulePresenter: load sikertelen");
        }
        refreshForProduct(productId);
        return ok;
    }

    // Hunglish: frissítés – adott Product GUID alapján kigyűjtjük a kapcsolt Materialokat
    void refreshForProduct(const QUuid& productId) {
        const auto rules = NeedRuleRegistry::instance().findByLeft(productId);

        QVector<MaterialRequirementsView::RequirementRow> rows;
        rows.reserve(rules.size());

        for (const auto& r : rules) {
            MaterialRequirementsView::RequirementRow row;
            row.product_id = r.leftId;
            row.material_id = r.rightId;

            // Hunglish: right() convenience – Material registry lookup
            if (auto matOpt = r.right()) {
                const auto& mat = *matOpt;
                row.material_name = mat.name;
                row.material_barcode = mat.barcode;
                row.material_exists = true;
            } else {
                row.material_name = "UNKNOWN";
                row.material_barcode = "";
                row.material_exists = false;
            }

            rows.append(row);
        }

        _view->set_requirements(rows);
        zInfo(QString("📊 NeedRulePresenter: %1 sor a view-ban Product=%2")
                       .arg(rows.size())
                       .arg(productId.toString(QUuid::WithoutBraces)));
    }

    // Hunglish: add – beszúrás registrybe, majd view refresh
    void addNeedRule(const QUuid& productId, const QUuid& materialId) {
        NeedRule c;
        c.leftId = productId;
        c.rightId = materialId;

        NeedRuleRegistry::instance().insert(c);
        zEventINFO(QString("➕ NeedRule add: %1").arg(c.describe()));

        // Optional: azonnali mentés (ha szeretnél), vagy későbbi save-all
        // NeedRuleRepository::save();

        refreshForProduct(productId);
    }

    // Hunglish: remove – törlés registryből, majd view refresh
    void removeNeedRule(const QUuid& productId, const QUuid& materialId) {
        const bool ok = NeedRuleRegistry::instance().remove(productId, materialId);
        if (ok) {
            zEventINFO(QString("🗑 NeedRule remove: left=%1 right=%2")
                           .arg(productId.toString(QUuid::WithoutBraces),
                                materialId.toString(QUuid::WithoutBraces)));
        } else {
            zWarning("⚠️ NeedRule remove: nem talált kapcsolat");
        }

        // Optional: azonnali mentés
        // NeedRuleRepository::save();

        refreshForProduct(productId);
    }
};
