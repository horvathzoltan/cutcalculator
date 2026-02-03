#pragma once

#include <QString>
#include <QVector>
#include "needs/model/need_rule.h"



class NeedRuleRepository {
    friend class test_NeedRuleRepository;
public:

    static bool load();
    static bool save();

    static QString toCsvLine(const NeedRule &r);
};

