#pragma once
#include <QString>
#include <QStringList>
#include "common/utils/filename_helper.h"
#include "materials/model/material_master.h"
#include "products/model/product_master.h"
//#include "needs/registry/need_rule_registry.h"

/**
 * 🧩 NeedRuleTraits – path és header policy
 *
 * * explicit előírjuk, hova perzisztáljon a NeedRule repo, és milyen legyen a fejléc.
 */

// Forward declaration – fontos!
class NeedRuleRegistry;

struct NeedRuleTraits {
    using LeftEntity = ProductMaster;
    using RightEntity = MaterialMaster;

    using RegistryType = NeedRuleRegistry;

    static QString filePath(FileAccess access) {
        QString csvPath = FileNameHelper::instance().pathFor(
            FileKind::NeedRules, access);
         return csvPath;
    }

    static QStringList headers() {
        return {"productId","materialId"};
    }
};
