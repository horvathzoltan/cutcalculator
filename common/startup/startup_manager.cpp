#include "startup_manager.h"

#include "common/utils/filename_helper.h"

// --- Registryk és repositoryk ---
#include "colors/registry/color_registry.h"
#include "colors/repository/color_repository.h"

#include "materials/registry/material_registry.h"
#include "materials/repository/material_repository.h"

#include "products/registry/product_registry.h"
#include "products/repository/product_repository.h"

#include "needs/registry/need_rule_registry.h"
#include "needs/repository/need_rule_repository.h"

#include "calcmodes/registry/need_calculation_registry.h"
#include "calcmodes/repository/need_calculation_repository.h"

#include "calculation/registry/need_calculation_detail_registry.h"
#include "calculation/repository/need_calculation_detail_repository.h"

#include "barcodes/registry/barcode_registry.h"
#include "barcodes/repository/barcode_repository.h"

// ============================================================
// 🧩 SPECIÁLIS LÉPÉSEK (nem generikusak)
// ============================================================
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

StartupStatus StartupManager::initNeedRuleRegistry()
{
    bool ok = NeedRuleRepository::load();

    if (!ok)
        return StartupStatus::failure("⚠️ NeedRuleRegistry: kapcsolatok betöltése sikertelen.");

    int count = NeedRuleRegistry::instance().size();

    if (count == 0)
        zInfo("📊 NeedRuleRegistry: jelenleg nincs kapcsolat – tiszta indulás");
    else
        zInfo(QString("📊 NeedRuleRegistry: %1 kapcsolat tárolva").arg(count));

    return StartupStatus::success();
}

// ============================================================
// 🚀 TELJES STARTUP PIPELINE
// ============================================================
StartupStatus StartupManager::runStartupSequence()
{
    QVector<StartupStatus> steps = {

    // 1) RAL színek (speciális)
    initRalColors(),

        // 2) Barcode
        initRegistryGeneric<BarcodeRegistry, BarcodeRepository>(
            "Barcode CSV betöltése sikertelen.", true
            ),

        // 3) Material
        initRegistryGeneric<MaterialRegistry, MaterialRepository>(
            "Material CSV betöltése sikertelen.", true
            ),

        // 4) Product
        initRegistryGeneric<ProductRegistry, ProductRepository>(
            "Product CSV betöltése sikertelen.", false
            ),

        // 5) NeedRule (speciális)
        initNeedRuleRegistry(),

        // 6) NeedCalculation (MÓKUSKA!)
        initRegistryGeneric<NeedCalculationRegistry, NeedCalculationRepository>(
            "NeedCalculation CSV betöltése sikertelen.", false
            ),

        // 7) NeedCalculationDetail (MÓKUSKA RÉSZLETEK!)
        initRegistryGeneric<NeedCalculationDetailRegistry, NeedCalculationDetailRepository>(
            "NeedCalculationDetail CSV betöltése sikertelen.", false
            )
};

StartupStatus finalStatus = StartupStatus::success();

for (const auto& s : steps) {
    if (!s.isSuccess())
        return s;
    finalStatus.addWarnings(s.warnings());
}

return finalStatus;
}
