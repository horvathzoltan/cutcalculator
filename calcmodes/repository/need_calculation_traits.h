#pragma once
#include <QString>
#include <QStringList>
#include "common/utils/filename_helper.h"
#include "calcmodes/model/need_calculation.h"
#include "calcmodes/registry/need_calculation_registry.h"

struct NeedCalculationTraits {
    using Entity = NeedCalculation;
    using RegistryType = NeedCalculationRegistry;

    static QString filePath() {
        return FileNameHelper::instance().getNeedCalculationCsvFile();
    }

    static QStringList headers() {
        return {"productDefinitionId", "modeName"};
    }
};
