#include "registry_initializer.h"

// --- Registryk include-ja ---
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"
#include "colors/registry/color_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "barcodes/registry/barcode_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"
// stb. – ide jön minden registry

void initializeAllRegistries()
{
    // Minden registry explicit inicializálása
    MaterialRegistry::instance().initialize();
    ProductRegistry::instance().initialize();
    ColorRegistry::instance().initialize();
    NeedRuleRegistry::instance().initialize();
    NeedCalculationRegistry::instance().initialize();
    //NeedCalculationDetailRegistry::instance().initialize();
    BarcodeRegistry::instance().initialize();

    // Ha később új registry jön, csak ide kell hozzáadni.
}
