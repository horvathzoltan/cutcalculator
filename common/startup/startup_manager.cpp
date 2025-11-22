#include "startup_manager.h"
#include "common/color/ral_importer.h"
#include "materials/repository/material_repository.h"
#include "materials/registry/material_registry.h"
//#include "materials/model/material_master.h"

#include <QSet>

#include "common/logger/event_logger.h"
//#include "common/logger/logger.h"
#include "common/utils/filename_helper.h"
//#include "common/color/namedcolor.h"

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

    zEvent(QString("🌱 Init összefoglaló: %1 anyag")
                                       .arg(MaterialRegistry::instance().readAll().size()));

    return finalStatus;
}

StartupStatus StartupManager::initMaterialRegistry() {
    bool loaded = MaterialRepository::loadFromCSV(MaterialRegistry::instance());
    if (!loaded){
        zError("❌ Nem sikerült betölteni az anyagtörzset");

        return StartupStatus::failure("❌ Nem sikerült betölteni az anyagtörzset a CSV fájlból.");
    }

    const auto& all = MaterialRegistry::instance().readAll();

    if (!hasMinimumMaterials(2)){
        zWarning("❌ túl kevés adat az anyagtörzsben");

        return StartupStatus::failure(
            QString("⚠️ Túl kevés anyag található a törzsben (%1 db). Legalább 2 szükséges.")
                .arg(all.size()));
    }

    zEvent(StatusHelper::getMessage(true,"anyagtörzs init"));
    return StartupStatus::success();
}


bool StartupManager::hasMinimumMaterials(int minCount) {
    return MaterialRegistry::instance().readAll().size() >= minCount;
}

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
        zError("❌ RAL színek betöltése sikertelen");

        return StartupStatus::failure("❌ Nem sikerült betölteni a RAL színeket a CSV fájlból.");
    }

    zEvent(StatusHelper::getMessage(true,"RAL színek init"));
    return StartupStatus::success();
}


