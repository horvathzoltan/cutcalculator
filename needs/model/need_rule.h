#pragma once
#include "needs/model/connection_entity.h"

/**
 * NeedRule
 *
 * Hunglish:
 * - Kapcsolótábla: Product ↔ Material
 * - Tiszta és karcsú: csak a két idegen kulcs (GUID).
 *
 * Elhelyezés:
 * - needs/model/need_rule.h
 */
struct NeedRule : public ConnectionEntity {
    // leftId = productId
    // rightId = materialId
};
