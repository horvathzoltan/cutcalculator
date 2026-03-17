// verbose_registry_initializer.cpp
#include "calcmodes/model/need_calculation.h"
#include "calculation/model/need_calculation_detail.h"
#include "common/system/verbose_manager.h"

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

    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<MaterialRegistry>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<BarcodeRegistry>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<ProductRepository>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<ProductRegistry>())), true);
    //vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<BarcodeTable>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<BarcodeValidator>())), true);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<BarcodeCollisionHelper>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryManager>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<TableFormatter>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<OverlayIconWidget>())), false);

    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryBase>())), true);


    // Konkrét engine specializációk
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryEngineBase<ProductMaster>>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryEngineBase<MaterialMaster>>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryEngineBase<NeedCalculation>>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryEngineBase<NeedCalculationDetail>>())), false);
    vm.registerClass(QString::fromStdString(std::string(nameof::nameof_type<RegistryEngineBase<ConnectionEntity<ProductMaster, MaterialMaster>>>())), false);

    // 🔍 Audit összefoglaló
    vm.dumpRegistry();
}
