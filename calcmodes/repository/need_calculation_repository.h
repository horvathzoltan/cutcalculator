#pragma once
#include <QVector>
#include "calcmodes/model/need_calculation.h"

/**
 * @class NeedCalculationRepository
 * @brief CSV import/export a NeedCalculation rekordokhoz.
 *
 * *
 * - egyszerű fejléc: productDefinitionId;modeName
 * - id-ket beolvasáskor generáljuk (UUID), audit log megy FileContextbe.
 */
class NeedCalculationRepository {
public:
    static bool load(QVector<NeedCalculation>& out);
    static bool save(const QVector<NeedCalculation>& data);
};
