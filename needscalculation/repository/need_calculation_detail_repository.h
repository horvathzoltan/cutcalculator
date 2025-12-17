#pragma once
#include <QVector>
#include "needscalculation/model/need_calculation_detail.h"

/**
 * @class NeedCalculationDetailRepository
 * @brief CSV import/export a NeedCalculationDetail rekordokhoz.
 *
 * Fejléc: needCalculationId;materialId;formula
 */
class NeedCalculationDetailRepository {
public:
    static bool load(QVector<NeedCalculationDetail>& out);
    static bool save(const QVector<NeedCalculationDetail>& data);
};
