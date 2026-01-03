#pragma once
#include <QString>
#include <QStringList>
#include "common/utils/filename_helper.h"
#include "needs/registry/need_rule_registry.h"

/**
 * 🧩 NeedRuleTraits – path és header policy
 *
 * * explicit előírjuk, hova perzisztáljon a NeedRule repo, és milyen legyen a fejléc.
 */
struct NeedRuleTraits {
    using LeftEntity = ProductMaster;
    using RightEntity = MaterialMaster;

    using RegistryType = NeedRuleRegistry;

    static QString filePath() {
        return FileNameHelper::instance().getNeedRuleCsvFile(); // pl. needs_rules.csv
    }
    static QStringList headers() {
        return {"productId","materialId"};
    }
};
