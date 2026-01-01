#pragma once
#include "connections/connection_repository.h"
#include "needs/model/need_rule.h"
#include "needs/repository/need_rule_traits.h"
#include "needs/registry/need_rule_registry.h"

/*
productId,materialId
ROL-RR,TE-R-23
ROL-RR,ROL-P
*/

/**
 * 📂 NeedRuleRepository – CSV import/export a NeedRule-hoz
 *
 * generikus repo sablonra épül, traits-ből kapja a pathot és headert.
 */
class NeedRuleRepository {
public:
    static bool load() {
        return ConnectionRepository<NeedRule, NeedRuleTraits>::load(NeedRuleRegistry::instance());
    }

    static bool save() {
        const auto& data = NeedRuleRegistry::instance().readAll();
        return ConnectionRepository<NeedRule, NeedRuleTraits>::save(data);
    }
};
