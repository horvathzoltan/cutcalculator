#pragma once
#include <QString>
#include <QVector>
#include "needs/model/need_rule.h"
#include "needs/registry/need_rule_registry.h"
#include "common/csv/csvimporter.h"
#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"

class NeedRuleRepository {
public:
    struct RawRow {
        QString productCode;
        QString materialCode;
        int lineNumber;
    };

    static bool load(QVector<NeedRule>& out);
    static bool save();
};
