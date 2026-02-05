#include "needs/repository/need_rule_repository.h"
#include "connections/connection_repository.h"
#include "needs/repository/need_rule_traits.h"
#include "needs/registry/need_rule_registry.h"

bool NeedRuleRepository::load(QVector<NeedRule>& out)
{
    return ConnectionRepository<NeedRule, NeedRuleTraits>::load(out);
}



bool NeedRuleRepository::save()
{
    return ConnectionRepository<NeedRule, NeedRuleTraits>::save(
        NeedRuleRegistry::instance()
        );
}

QString NeedRuleRepository::toCsvLine(const NeedRule &r)
{
    QString e = ConnectionRepository<NeedRule, NeedRuleTraits>::toCsvLine(r);
    return e;
}
