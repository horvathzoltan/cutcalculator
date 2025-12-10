#include <QSet>

#include "startup_manager.h"
#include "colors/repository/color_repository.h"
#include "common/utils/filename_helper.h"

#include "materials/repository/material_repository.h"
#include "materials/registry/material_registry.h"
#include "products/repository/product_repository.h"
#include "products/registry/product_registry.h"

#include "needs/repository/need_rule_repository.h"
#include "needs/registry/need_rule_registry.h"

StartupStatus StartupManager::runStartupSequence() {
    StartupStatus ralColorStatus = initRalColors();
    if (!ralColorStatus.isSuccess())
        return ralColorStatus;

    StartupStatus materialStatus = initMaterialRegistry();
    if (!materialStatus.isSuccess())
        return materialStatus;

    StartupStatus productStatus = initProductRegistry();
    if (!productStatus.isSuccess())
        return productStatus;

    StartupStatus needRuleStatus = initNeedRuleRegistry();
    if (!needRuleStatus.isSuccess())
        return needRuleStatus;

    StartupStatus finalStatus = StartupStatus::success();
    finalStatus.addWarnings(ralColorStatus.warnings());
    finalStatus.addWarnings(materialStatus.warnings());
    finalStatus.addWarnings(productStatus.warnings());
    finalStatus.addWarnings(needRuleStatus.warnings());

    return finalStatus;
}

StartupStatus StartupManager::initMaterialRegistry() {
    bool loaded = MaterialRepository::loadFromCSV(MaterialRegistry::instance());
    if (loaded){
        zInfo(QString("📊 MaterialRegistry: %1 anyag tárolva").arg(MaterialRegistry::instance().size()));
    } else{
        return StartupStatus::failure("⚠️ MaterialRegistry: anyagok betöltése sikertelen.");
    }

    return StartupStatus::success();
}

StartupStatus StartupManager::initProductRegistry() {
    bool loaded = ProductRepository::loadFromCSV(ProductRegistry::instance());
    if (loaded) {
        zInfo(QString("📊 ProductRegistry: %1 terméktípus tárolva")
                  .arg(ProductRegistry::instance().size()));
    } else {
        return StartupStatus::failure("⚠️ ProductRegistry: terméktípusok betöltése sikertelen.");
    }
    return StartupStatus::success();
}

// bool StartupManager::hasMinimumMaterials(int minCount) {
//     return MaterialRegistry::instance().readAll().size() >= minCount;
// }

StartupStatus StartupManager::initRalColors()
{
    auto fnh = FileNameHelper::instance();

    QList<RalSource> ralSources = {
        { RalSystem::Classic,  fnh.getRalClassicCsvFile() },
        { RalSystem::Design,   fnh.getRalDesignCsvFile() },
        { RalSystem::Plastic1, fnh.getRalPlastic1CsvFile() },
        { RalSystem::Plastic2, fnh.getRalPlastic2CsvFile() }
    };

    bool loaded = ColorRepository::loadRalColors({ralSources});
    if (!loaded){
        return StartupStatus::failure("❌ Nem sikerült betölteni a RAL színeket a CSV fájlból.");
    }

    return StartupStatus::success();
}


StartupStatus StartupManager::initNeedRuleRegistry() {
    bool loaded = NeedRuleRepository::load();
    if (!loaded) {
        return StartupStatus::failure("⚠️ NeedRuleRegistry: kapcsolatok betöltése sikertelen.");
    }

    int count = NeedRuleRegistry::instance().size();
    if (count == 0) {
        zEventINFO("📊 NeedRuleRegistry: jelenleg nincs kapcsolat – tiszta indulás");
    } else {
        zEventINFO(QString("📊 NeedRuleRegistry: %1 kapcsolat tárolva").arg(count));
    }

    return StartupStatus::success();
}
