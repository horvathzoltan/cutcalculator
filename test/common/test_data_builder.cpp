#include "test_data_builder.h"

#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"
//#include "needs/registry/need_rule_registry.h"

TestDataIds TestDataBuilder::prepareStandard()
{
    TestDataIds ids;

    auto& preg = ProductRegistry::instance();
    auto& mreg = MaterialRegistry::instance();
    //auto& nreg = NeedRuleRegistry::instance();

    preg.clearForTest();
    mreg.clearForTest();
    //nreg.clear();

    // --- Product ---
    ProductMaster p;
    p.id = ids.P1 = QUuid::createUuid();
    p.name = "Prod1";
    p.barcode = "P1";
    preg.insert(p);

    // --- Material 1 ---
    MaterialMaster m;
    m.id = ids.M1 = QUuid::createUuid();
    m.name = "Mat1";
    m.barcode = "M1";
    mreg.addForTest(m);

    // --- Material 2 ---
    MaterialMaster m2;
    m2.id = ids.M2 = QUuid::createUuid();
    m2.name = "Mat2";
    m2.barcode = "M2";
    mreg.addForTest(m2);

    return ids;
}
