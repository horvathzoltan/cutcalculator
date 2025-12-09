#pragma once
#include <QUuid>

/**
 * ConnectionEntity
 *
 * Hunglish:
 * - Közös alap minden kapcsolótáblához (NeedRule, NeedCalculationDetail, stb.)
 * - Tiszta kétirányú kapcsolat: leftId ↔ rightId
 * - Nincs saját id, mert a két kulcs együtt természetes azonosító.
 *
 * Elhelyezés:
 * - needs/model/connection_entity.h
 */
struct ConnectionEntity {
    QUuid leftId;   // pl. productId vagy needCalculationId
    QUuid rightId;  // pl. materialId
};
