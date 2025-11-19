#include "startup_manager.h"
#include "common/color/ral_importer.h"
#include "materials/repository/material_repository.h"
#include "materials/registry/material_registry.h"
//#include "materials/model/material_master.h"

#include <QSet>

#include "common/logger/event_logger.h"
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

    EventLogger::instance().zEvent(QString("🌱 Init összefoglaló: %1 anyag, %2 gép, %3 stock, %4 leftover")
                                       .arg(MaterialRegistry::instance().readAll().size()));

    return finalStatus;
}

StartupStatus StartupManager::initMaterialRegistry() {
    bool loaded = MaterialRepository::loadFromCSV(MaterialRegistry::instance());
    if (!loaded){
        EventLogger::instance().zEvent("❌ Nem sikerült betölteni az anyagtörzset");

        return StartupStatus::failure("❌ Nem sikerült betölteni az anyagtörzset a CSV fájlból.");
    }

    const auto& all = MaterialRegistry::instance().readAll();

    if (!hasMinimumMaterials(2)){
        EventLogger::instance().zEvent("❌ túl kevés adat az anyagtörzsben");

        return StartupStatus::failure(
            QString("⚠️ Túl kevés anyag található a törzsben (%1 db). Legalább 2 szükséges.")
                .arg(all.size()));
    }

    StartupStatus status = StartupStatus::success();

    EventLogger::instance().zEvent(StatusHelper::getMessage(status.isSuccess(),"anyagtörzs init"));
    return status;
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

    QStringList failedSystems;
    StartupStatus status = StartupStatus::success();

    for (const auto& src : ralSources) {
        CsvImporter::FileContext ctx(src.filePath);

        bool ok = RalImporter::loadRalColors({src});
        if (!ok || ctx.hasErrors()) {
            QString sysName = RalSystemUtils::toString(src.system);
            failedSystems << sysName;

            QString msg = QString("❌ RAL import hiba [%1]: %2")
                              .arg(sysName)
                              .arg(src.filePath);
            EventLogger::instance().zEvent(msg);

            status.addWarning(ctx.hasErrors() ? ctx.toString() : msg);
        } else {
            EventLogger::instance().zEvent(
                QString("✅ RAL import sikeres [%1]: %2")
                    .arg(RalSystemUtils::toString(src.system))
                    .arg(src.filePath));
        }
    }

    if (!status.warnings().isEmpty()) {
        QString summary = QString("❌ RAL színek inicializálása sikertelen a következő rendszereknél: %1")
                              .arg(failedSystems.join(", "));
        return StartupStatus::failure(summary);
    }

    EventLogger::instance().zEvent(StatusHelper::getMessage(true,"RAL színek init"));
    return StartupStatus::success();
}


