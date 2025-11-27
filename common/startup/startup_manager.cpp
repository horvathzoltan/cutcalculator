#include "startup_manager.h"
#include "common/color/ral_importer.h"
#include "materials/repository/material_repository.h"
#include "materials/registry/material_registry.h"
#include <QSet>
#include "common/utils/filename_helper.h"

StartupStatus StartupManager::runStartupSequence() {
    StartupStatus ralColorStatus = initRalColors();
    if (!ralColorStatus.isSuccess())
        return ralColorStatus;

    StartupStatus materialStatus = initMaterialRegistry();
    if (!materialStatus.isSuccess())
        return materialStatus; 

    StartupStatus finalStatus = StartupStatus::success();
    finalStatus.addWarnings(ralColorStatus.warnings());
    finalStatus.addWarnings(materialStatus.warnings());

    return finalStatus;
}

StartupStatus StartupManager::initMaterialRegistry() {
    bool loaded = MaterialRepository::loadFromCSV(MaterialRegistry::instance());
    if (loaded){
        zInfo(QString("📊 MaterialRegistry: %1 anyag tárolva").arg(MaterialRegistry::instance().size()));
    } else{
        return StartupStatus::failure("⚠️ MaterialRegistry: anyagok betöltése sikertelen.");
    }

    //const auto& all = MaterialRegistry::instance().readAll();
    // if (!hasMinimumMaterials(2)){
    //     return StartupStatus::failure(
    //         QString("⚠️ Túl kevés anyag található a törzsben (%1 db). Legalább 2 szükséges.")
    //             .arg(all.size()));
    // }

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

    bool loaded = RalImporter::loadRalColors({ralSources});
    if (!loaded){
        return StartupStatus::failure("❌ Nem sikerült betölteni a RAL színeket a CSV fájlból.");
    }

    return StartupStatus::success();
}


