#include <QSet>

#include "colors/registry/color_registry.h"

#include "startup_manager.h"
#include "colors/repository/color_repository.h"
#include "common/utils/filename_helper.h"

#include "materials/repository/material_repository.h"
#include "materials/registry/material_registry.h"
#include "products/repository/product_repository.h"
#include "products/registry/product_registry.h"
#include "needs/repository/need_rule_repository.h"
#include "needs/registry/need_rule_registry.h"

#include "barcodes/repository/barcode_repository.h"
#include "barcodes/registry/barcode_registry.h"

StartupStatus StartupManager::runStartupSequence() {
    StartupStatus ralColorStatus = initRalColors();
    if (!ralColorStatus.isSuccess())
        return ralColorStatus;

    StartupStatus barcodeStatus = initBarcodeRegistry();
    if (!barcodeStatus.isSuccess())
        return barcodeStatus;

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
    finalStatus.addWarnings(barcodeStatus.warnings());
    finalStatus.addWarnings(materialStatus.warnings());
    finalStatus.addWarnings(productStatus.warnings());
    finalStatus.addWarnings(needRuleStatus.warnings());

    return finalStatus;
}

// StartupStatus StartupManager::initMaterialRegistry() {
//     bool loaded = MaterialRepository::loadFromCSV(MaterialRegistry::instance());
//     if (loaded){
//         zInfo(QString("📊 MaterialRegistry: %1 anyag tárolva").arg(MaterialRegistry::instance().size()));
//     } else{
//         return StartupStatus::failure("⚠️ MaterialRegistry: anyagok betöltése sikertelen.");
//     }

//     return StartupStatus::success();
// }

StartupStatus StartupManager::initMaterialRegistry() {
    QVector<MaterialMaster> mats;
    bool ok = MaterialRepository::load(mats);

    if (!ok)
        return StartupStatus::failure("Material CSV betöltése sikertelen.");

    MaterialRegistry::instance().setAll(mats);

    zInfo(QString("📦 MaterialRegistry: %1 anyag betöltve")
              .arg(MaterialRegistry::instance().size()));

    return StartupStatus::success();
}


StartupStatus StartupManager::initProductRegistry() {
    QVector<ProductMaster> products;
    bool ok = ProductRepository::load(products);

    if (!ok)
        return StartupStatus::failure("Product CSV betöltése sikertelen.");

    ProductRegistry::instance().setAll(products);

    zInfo(QString("📦 ProductRegistry: %1 termék betöltve")
              .arg(ProductRegistry::instance().size()));

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

    QVector<NamedColor> colors;
    bool ok = ColorRepository::loadRalColors(ralSources, colors);

    if (!ok)
        return StartupStatus::failure("❌ Nem sikerült betölteni a RAL színeket.");

    ColorRegistry::instance().setAll(colors);

    zInfo(QString("🎨 ColorRegistry: %1 szín betöltve")
              .arg(ColorRegistry::instance().size()));

    return StartupStatus::success();
}

// StartupStatus StartupManager::initRalColors()
// {
//     auto fnh = FileNameHelper::instance();

//     QList<RalSource> ralSources = {
//         { RalSystem::Classic,  fnh.getRalClassicCsvFile() },
//         { RalSystem::Design,   fnh.getRalDesignCsvFile() },
//         { RalSystem::Plastic1, fnh.getRalPlastic1CsvFile() },
//         { RalSystem::Plastic2, fnh.getRalPlastic2CsvFile() }
//     };

//     bool loaded = ColorRepository::loadRalColors({ralSources});
//     if (!loaded){
//         return StartupStatus::failure("❌ Nem sikerült betölteni a RAL színeket a CSV fájlból.");
//     }

//     return StartupStatus::success();
// }

StartupStatus StartupManager::initNeedRuleRegistry() {
    bool ok = NeedRuleRepository::load();

    if (!ok) {
        return StartupStatus::failure("⚠️ NeedRuleRegistry: kapcsolatok betöltése sikertelen.");
    }

    int count = NeedRuleRegistry::instance().size();
    if (count == 0) {
        zInfo("📊 NeedRuleRegistry: jelenleg nincs kapcsolat – tiszta indulás");
    } else {
        zInfo(QString("📊 NeedRuleRegistry: %1 kapcsolat tárolva").arg(count));
    }

    return StartupStatus::success();
}

// StartupStatus StartupManager::initNeedRuleRegistry() {
//     QVector<NeedRule> rules;
//     bool ok = NeedRuleRepository::load(rules);

//     if (!ok) {
//         return StartupStatus::failure("⚠️ NeedRuleRegistry: kapcsolatok betöltése sikertelen.");
//     }

//     NeedRuleRegistry::instance().setAll(rules);

//     int count = NeedRuleRegistry::instance().size();
//     if (count == 0) {
//         zInfo("📊 NeedRuleRegistry: jelenleg nincs kapcsolat – tiszta indulás");
//     } else {
//         zInfo(QString("📊 NeedRuleRegistry: %1 kapcsolat tárolva").arg(count));
//     }

//     return StartupStatus::success();
// }

// StartupStatus StartupManager::initBarcodeRegistry() {
//     bool loaded = BarcodeRepository::loadFromCSV(BarcodeRegistry::instance());
//     if (loaded) {
//         zInfo(QString("📊 BarcodeRegistry: %1 rekord tárolva")
//                   .arg(BarcodeRegistry::instance().size()));
//     } else {
//         return StartupStatus::failure("⚠️ BarcodeRegistry: barcodes betöltése sikertelen.");
//     }
//     return StartupStatus::success();
// }
StartupStatus StartupManager::initBarcodeRegistry() {
    QVector<BarcodeRecord> records;
    bool ok = BarcodeRepository::load(records);

    if (!ok)
        return StartupStatus::failure("⚠️ Barcode CSV betöltése sikertelen.");

    BarcodeRegistry::instance().setAll(records);

    zInfo(QString("📊 BarcodeRegistry: %1 rekord betöltve")
              .arg(BarcodeRegistry::instance().size()));

    return StartupStatus::success();
}

