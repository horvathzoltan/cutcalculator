#pragma once
#include <QObject>
#include "needs/view/material_requirements_view.h"
#include "needs/registry/need_rule_registry.h"

/**
 * MaterialRequirementsManager
 *
 * *
 * - Glue a view és a registry/repo között.
 * - Kezeli az add/remove kéréseket, és visszatölti a táblát.
 * - A product selection váltására szűrten tölti a kapcsolatok listáját.
 *
 * Elhelyezés:
 * - needs/manager/material_requirements_manager.h/.cpp
 */
class MaterialRequirementsManager : public QObject {
    Q_OBJECT
public:
    explicit MaterialRequirementsManager(MaterialRequirementsView* view, QObject* parent = nullptr);

    // explicit refresh egy productra
    void refreshForProduct(const QUuid& productId,
                           const QString& productName,
                           const QString& productBarcode);

private:
    MaterialRequirementsView* _view = nullptr;

    void connectSignals();

    // Helper: registry → view rows
    QVector<MaterialRequirementsView::RequirementRow>
    makeRowsForProduct(const QUuid& productId,
                       const QString& productName,
                       const QString& productBarcode);
};
