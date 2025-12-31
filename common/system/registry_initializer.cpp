#include "registry_initializer.h"

// --- Registryk include-ja ---
#include "materials/registry/material_registry.h"
#include "products/registry/product_registry.h"
#include "colors/registry/color_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "barcodes/registry/barcode_registry.h"
// stb. – ide jön minden registry

void initializeAllRegistries()
{
    // Minden registry explicit inicializálása
    MaterialRegistry::initializeSingleton();
    ProductRegistry::initializeSingleton();
//    ColorRegistry::initializeSingleton();
//    NeedRuleRegistry::initializeSingleton();
//    NeedCalculationRegistry::initializeSingleton();
//    BarcodeRegistry::initializeSingleton();

    // Ha később új registry jön, csak ide kell hozzáadni.
}
