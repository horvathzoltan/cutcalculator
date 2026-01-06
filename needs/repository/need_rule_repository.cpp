#include "needs/repository/need_rule_repository.h"
#include "connections/connection_repository.h"
#include "needs/repository/need_rule_traits.h"
#include "needs/registry/need_rule_registry.h"

bool NeedRuleRepository::load()
{
    return ConnectionRepository<NeedRule, NeedRuleTraits>::load(
        NeedRuleRegistry::instance()
        );
}

bool NeedRuleRepository::save()
{
    return ConnectionRepository<NeedRule, NeedRuleTraits>::save(
        NeedRuleRegistry::instance()
        );
}
