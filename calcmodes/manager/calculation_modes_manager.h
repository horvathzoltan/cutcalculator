#pragma once

#include <QObject>
#include <QUuid>

#include "calcmodes/view/calculation_modes_view.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "calcmodes/repository/need_calculation_repository.h"
#include "calculation/repository/need_calculation_detail_repository.h"

/* ============================================================
 * 🧩 CalculationModesManager
 * ------------------------------------------------------------
 * Glue réteg:
 *  - termék kiválasztása → számítási módok listája
 *  - CRUD műveletek (add / remove / rename)
 *  - perzisztencia (CSV load)
 *  - nézet frissítése
 * ============================================================ */
class CalculationModesManager : public QObject {
    Q_OBJECT

public:
    /* ============================================================
     * 🧩 Konstruktor
     * ============================================================ */
    explicit CalculationModesManager(CalculationModesView* view,
                                     QObject* parent = nullptr);

    /* ============================================================
     * 🧩 Public API
     * ============================================================ */
    void refreshForProduct(const QUuid& productId,
                           const QString& productName,
                           const QString& productBarcode);

private:
    /* ============================================================
     * 🧩 Private members
     * ============================================================ */
    CalculationModesView* _view = nullptr;

    /* ============================================================
     * 🧩 🔧 Privát segédfüggvények
     * ============================================================ */
    void connectSignals();
    void reloadAll();
};
