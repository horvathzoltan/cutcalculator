// verbose_registry_initializer.cpp
#include "calcmodes/model/need_calculation.h"
#include "calculation/model/need_calculation_detail.h"
#include "common/system/verbose_manager.h"
#include "verbose_class_initializer.h"
#include "common/utils/geometry_helper.h"
#include "window_stability_monitor.h"

// 🔍 Minden regisztrált osztály includeolva – így a fordító validálja a típusneveket
#include "connections/connection_entity.h"
#include "materials/registry/material_registry.h"
#include "products/repository/product_repository.h"
#include "products/registry/product_registry.h"
#include "barcodes/validator/barcode_validator.h"
#include "barcodes/helpers/barcode_collision_helper.h"
#include "common/registry/manager/registry_manager.h"
#include "common/utils/table_formatter.h"
#include "ui/widgets/overlay_icon_widget.h"

// 🧠 Kowalski: központi regisztrációs stratégia
void registerAllVerbose() {
    auto& vm = VerboseManager::instance();

    // 🧩 Típuslista – bővíthető, átlátható
    const QVector<VerboseEntry> entries = {
        VERBOSE_OFF<MaterialRegistry>(),
        VERBOSE_OFF<BarcodeRegistry>(),
        VERBOSE_OFF<ProductRepository>(),
        VERBOSE_ON<ProductRegistry>(),
        VERBOSE_ON<BarcodeValidator>(),
        VERBOSE_OFF<BarcodeCollisionHelper>(),
        VERBOSE_OFF<RegistryManager>(),
        VERBOSE_OFF<TableFormatter>(),
        VERBOSE_OFF<OverlayIconWidget>(),
        VERBOSE_ON<RegistryBase>(),
        VERBOSE_OFF<RegistryEngineBase<BarcodeRecord>>(),

        // Engine specializációk
        VERBOSE_OFF<RegistryEngineBase<ProductMaster>>(),
        VERBOSE_OFF<RegistryEngineBase<MaterialMaster>>(),
        VERBOSE_OFF<RegistryEngineBase<NeedCalculation>>(),
        VERBOSE_OFF<RegistryEngineBase<NeedCalculationDetail>>(),
        VERBOSE_OFF<RegistryEngineBase<ConnectionEntity<ProductMaster, MaterialMaster>>>(),
        VERBOSE_ON<WindowStabilityMonitor>(),
        VERBOSE_ON<GeometryHelper>()
    };

    // 🔁 Egyetlen ciklus — tiszta, clazy‑barát
    for (const auto& e : entries) {
        vm.registerClass(e.className, e.verbose);
    }
    // 🔍 Audit összefoglaló
    vm.dumpRegistry();
}
