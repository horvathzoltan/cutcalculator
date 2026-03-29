#include "test_roletta_builder.h"

#include "products/model/product_master.h"
#include "materials/model/material_master.h"
#include "calcmodes/model/need_calculation.h"
#include "calculation/model/need_calculation_detail.h"

TestRolettaBuilder::Ids TestRolettaBuilder::build()
{
    Ids ids;

    // 1) Registryk törlése (test mode)
    ProductRegistry::instance().clearForTest();
    MaterialRegistry::instance().clearForTest();
    NeedRuleRegistry::instance().clearForTest();
    NeedCalculationRegistry::instance().clearForTest();
    NeedCalculationDetailRegistry::instance().clearForTest();

    // ---------------------------------------------------------
    // 2) Product tree (hierarchy)
    // ---------------------------------------------------------

    // Root: "ROL" (Roletta)
    QUuid root_id = QUuid::createUuid();
    {
        ProductMaster root;
        root.id = root_id;
        root.name = "Roletta";
        root.barcode = "ROL";
        root.parentId = QUuid();        // root
        ProductRegistry::instance().insert(root);
    }

    // Rugós roletta
    {
        ProductMaster p;
        p.id = QUuid::createUuid();
        p.name = "Rugós roletta";
        p.barcode = "ROL-R";
        p.parentId = root_id;
        ProductRegistry::instance().insert(p);
        ids.ROL_R = p.id;
    }

    // Tetőtéri roletta
    {
        ProductMaster p;
        p.id = QUuid::createUuid();
        p.name = "Tetőtéri roletta";
        p.barcode = "ROL-RT";
        p.parentId = root_id;
        ProductRegistry::instance().insert(p);
        ids.ROL_RT = p.id;
    }

    // Gyöngyös alap
    {
        ProductMaster p;
        p.id = QUuid::createUuid();
        p.name = "Gyöngyös 23 (Alap)";
        p.barcode = "GY-A23";
        p.parentId = root_id;
        ProductRegistry::instance().insert(p);
        ids.GY_A23 = p.id;
    }

    // ---------------------------------------------------------
    // 3) Materials
    // ---------------------------------------------------------

    auto add_material = [&](const QString& bc, const QString& name) {
        MaterialMaster m;
        m.id = QUuid::createUuid();
        m.name = name;
        m.barcode = bc;
        MaterialRegistry::instance().addForTest(m);
        return m.id;
    };

    ids.TE_R_23      = add_material("TE-R-23", "Ø23 ragacsos tengely");
    ids.ROL_P        = add_material("ROL-P", "Alsópálca");
    ids.ROL_RRG      = add_material("ROL-RRG", "Rugós rugó");
    ids.ROL_TRG      = add_material("ROL-TRG", "Tetőtéri rugó");
    ids.ROL_GYZ      = add_material("ROL-GYZ", "Gyöngyös záró");
    ids.ROL_ENDCAP   = add_material("ROL-ENDCAP", "Tengelyvég dugó");
    ids.ROL_P_ENDCAP = add_material("ROL-P-ENDCAP", "Alsópálca dugó");
    ids.ROL_FABRIC   = add_material("ROL-FABRIC", "Vászon");

    // ---------------------------------------------------------
    // 4) NeedRule BOM connections
    // ---------------------------------------------------------

    auto add_rule = [&](QUuid product, QUuid material) {
        NeedRuleRegistry::instance().insert(product, material);
    };

    // Rugós
    add_rule(ids.ROL_R, ids.TE_R_23);
    add_rule(ids.ROL_R, ids.ROL_P);
    add_rule(ids.ROL_R, ids.ROL_RRG);
    add_rule(ids.ROL_R, ids.ROL_ENDCAP);
    add_rule(ids.ROL_R, ids.ROL_P_ENDCAP);
    add_rule(ids.ROL_R, ids.ROL_FABRIC);

    // Tetőtéri
    add_rule(ids.ROL_RT, ids.TE_R_23);
    add_rule(ids.ROL_RT, ids.ROL_P);
    add_rule(ids.ROL_RT, ids.ROL_TRG);
    add_rule(ids.ROL_RT, ids.ROL_ENDCAP);
    add_rule(ids.ROL_RT, ids.ROL_P_ENDCAP);
    add_rule(ids.ROL_RT, ids.ROL_FABRIC);

    // Gyöngyös alap
    add_rule(ids.GY_A23, ids.TE_R_23);
    add_rule(ids.GY_A23, ids.ROL_P);
    add_rule(ids.GY_A23, ids.ROL_GYZ);
    add_rule(ids.GY_A23, ids.ROL_ENDCAP);
    add_rule(ids.GY_A23, ids.ROL_P_ENDCAP);
    add_rule(ids.GY_A23, ids.ROL_FABRIC);

    // ---------------------------------------------------------
    // 5) Manufacturing calculation modes – mindhárom termékhez külön
    // ---------------------------------------------------------

    auto add_calc = [&](const QUuid& productId) {
        NeedCalculation calc;
        calc.id = QUuid::createUuid();
        calc.productId = productId;
        calc.name = "Manufacturing";
        NeedCalculationRegistry::instance().insert(calc);
        return calc.id;
    };

    ids.calcManufacturingRolR   = add_calc(ids.ROL_R);
    ids.calcManufacturingRolRT  = add_calc(ids.ROL_RT);
    ids.calcManufacturingGyA23  = add_calc(ids.GY_A23);

    // ---------------------------------------------------------
    // 6) Formulas (Cutting + Kitting)
    // ---------------------------------------------------------

    auto add_detail = [&](QUuid calcId,
                          QUuid mat,
                          const QString& formula,
                          NeedCalculationDetail::DetailKind kind)
    {
        NeedCalculationDetail d;
        d.id = QUuid::createUuid();
        d.needCalculationId = calcId;
        d.materialId = mat;
        d.formula = formula;
        d.kind = kind;
        NeedCalculationDetailRegistry::instance().insert(d);
    };

    // --- Cutting DSL – mindhárom módra ugyanaz a geometria ---

    auto add_common_cutting = [&](QUuid calcId) {
        add_detail(calcId, ids.TE_R_23,    "len = w - 30", NeedCalculationDetail::DetailKind::Cutting);
        add_detail(calcId, ids.ROL_P,      "len = w - 30", NeedCalculationDetail::DetailKind::Cutting);
        add_detail(calcId, ids.ROL_FABRIC, "len = h - 20", NeedCalculationDetail::DetailKind::Cutting);
        // ⚠️ Dugók NEM Cutting! (különben nem kerülnek be a kitItems-be)
    };

    add_common_cutting(ids.calcManufacturingRolR);
    add_common_cutting(ids.calcManufacturingRolRT);
    add_common_cutting(ids.calcManufacturingGyA23);

    // --- Kitting DSL ---

    auto add_common_kitting = [&](QUuid calcId) {
        // Alsópálca dugó: 2 db
        add_detail(calcId, ids.ROL_P_ENDCAP,
                   "qty = 2",
                   NeedCalculationDetail::DetailKind::Kitting);

        // Tengelyvég dugó: 2 db
        add_detail(calcId, ids.ROL_ENDCAP,
                   "qty = 2",
                   NeedCalculationDetail::DetailKind::Kitting);
    };

    // Rugós: rugós rugó legyen
    add_common_kitting(ids.calcManufacturingRolR);
    add_detail(ids.calcManufacturingRolR, ids.ROL_RRG,
               "qty = 1",
               NeedCalculationDetail::DetailKind::Kitting);

    // Tetőtéri: tetőtéri rugó legyen
    add_common_kitting(ids.calcManufacturingRolRT);
    add_detail(ids.calcManufacturingRolRT, ids.ROL_TRG,
               "qty = 1",
               NeedCalculationDetail::DetailKind::Kitting);

    // Gyöngyös: gyöngyös záró legyen
    add_common_kitting(ids.calcManufacturingGyA23);
    add_detail(ids.calcManufacturingGyA23, ids.ROL_GYZ,
               "qty = 1",
               NeedCalculationDetail::DetailKind::Kitting);

    return ids;
}
