// test/calculation/roletta_calculation_tester.cpp
#include "roletta_calculation_tester.h"

#include <algorithm>

#include "calculation/service/need_calculator.h"
#include "calculation/registry/need_calculation_detail_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"

namespace {

// 🔎 Segédfüggvény: vágási tétel keresése barcode alapján
const RawCut* find_cut_by_barcode(const ItemNeed& need, const QString& barcode)
{
    for (const RawCut& c : need.cutItems) {
        if (c.materialBarcode == barcode)
            return &c;
    }
    return nullptr;
}

// 🔎 Segédfüggvény: kitting tétel keresése barcode alapján
const RawKit* find_kit_by_barcode(const ItemNeed& need, const QString& barcode)
{
    for (const RawKit& k : need.kitItems) {
        if (k.materialBarcode == barcode)
            return &k;
    }
    return nullptr;
}

// 🔎 Segédfüggvény: van‑e legalább egy Cutting / Kitting detail
bool has_detail_kind(const QVector<NeedCalculationDetail>& details,
                     NeedCalculationDetail::DetailKind kind)
{
    for (const NeedCalculationDetail& d : details) {
        if (d.kind == kind)
            return true;
    }
    return false;
}

} // namespace

bool RolettaCalculationTester::run()
{
    // 1) Roletta tesztadatok felépítése
    TestRolettaBuilder::Ids ids = TestRolettaBuilder::build();

    // 2) Először magát a buildert validáljuk – fa, BOM, módok, formulák
    verify_builder(ids);

    // 3) Konkrét roletta számítási forgatókönyvek
    test_rugos_roletta(ids);
    test_tetoteri_roletta(ids);
    test_alap_roletta(ids);

    return true;
}

// 🧪 TestRolettaBuilder sanity check – termékfa, anyagok, BOM, Manufacturing mód
void RolettaCalculationTester::verify_builder(const TestRolettaBuilder::Ids& ids)
{
    auto& product_reg = ProductRegistry::instance();
    auto& material_reg = MaterialRegistry::instance();
    auto& rule_reg = NeedRuleRegistry::instance();
    auto& calc_reg = NeedCalculationRegistry::instance();
    auto& detail_reg = NeedCalculationDetailRegistry::instance();

    // --- Termékek léteznek ---
    Q_ASSERT(product_reg.findById(ids.ROL_R)  != nullptr);
    Q_ASSERT(product_reg.findById(ids.ROL_RT) != nullptr);
    Q_ASSERT(product_reg.findById(ids.GY_A23) != nullptr);

    // --- Anyagok léteznek ---
    Q_ASSERT(material_reg.findById(ids.TE_R_23)      != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_P)        != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_RRG)      != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_TRG)      != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_GYZ)      != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_ENDCAP)   != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_P_ENDCAP) != nullptr);
    Q_ASSERT(material_reg.findById(ids.ROL_FABRIC)   != nullptr);

    // --- BOM kapcsolatok ---
    Q_ASSERT(!rule_reg.findByLeft(ids.ROL_R).isEmpty());
    Q_ASSERT(!rule_reg.findByLeft(ids.ROL_RT).isEmpty());
    Q_ASSERT(!rule_reg.findByLeft(ids.GY_A23).isEmpty());

    auto check_calc = [&](const QUuid& calcId) {
        const NeedCalculation* calc = calc_reg.findById(calcId);
        Q_ASSERT(calc != nullptr);
        Q_ASSERT(calc->name == QStringLiteral("Manufacturing"));

        const QVector<NeedCalculationDetail> details =
            detail_reg.findByCalculation(calcId);
        Q_ASSERT(!details.isEmpty());
        Q_ASSERT(has_detail_kind(details, NeedCalculationDetail::DetailKind::Cutting));
        Q_ASSERT(has_detail_kind(details, NeedCalculationDetail::DetailKind::Kitting));
    };

    check_calc(ids.calcManufacturingRolR);
    check_calc(ids.calcManufacturingRolRT);
    check_calc(ids.calcManufacturingGyA23);
}


// 🧪 Rugós roletta – ROL_R
void RolettaCalculationTester::test_rugos_roletta(const TestRolettaBuilder::Ids& ids)
{
    OrderLine line;
    line.productId  = ids.ROL_R;
    line.width_mm   = 1200;
    line.height_mm  = 1500;
    line.handlerSide = QStringLiteral("L");

    ItemNeed need = NeedCalculator::calculate(line, QStringLiteral("Manufacturing"), true);

    // --- Cutting DSL elvárások ---
    // Tengely: requiredLength = w - 30
    const RawCut* shaft_cut =
        find_cut_by_barcode(need, QStringLiteral("TE-R-23"));
    Q_ASSERT(shaft_cut != nullptr);
    Q_ASSERT(shaft_cut->requiredLength == line.width_mm - 30);

    // Alsópálca: requiredLength = w - 30
    const RawCut* bottom_bar_cut =
        find_cut_by_barcode(need, QStringLiteral("ROL-P"));
    Q_ASSERT(bottom_bar_cut != nullptr);
    Q_ASSERT(bottom_bar_cut->requiredLength == line.width_mm - 30);

    // Vászon: requiredLength = h - 20
    const RawCut* fabric_cut =
        find_cut_by_barcode(need, QStringLiteral("ROL-FABRIC"));
    Q_ASSERT(fabric_cut != nullptr);
    Q_ASSERT(fabric_cut->requiredLength == line.height_mm - 20);

    // --- Kitting DSL elvárások ---
    // Minden rolettához legyen legalább 2 kit elem
    Q_ASSERT(need.kitItems.size() >= 2);

    // Alsópálca dugó: 2 db
    const RawKit* bottom_cap_kit =
        find_kit_by_barcode(need, QStringLiteral("ROL-P-ENDCAP"));
    Q_ASSERT(bottom_cap_kit != nullptr);
    Q_ASSERT(bottom_cap_kit->kitting_qty == 2);

    // Rugós rolettánál RRG rugó legyen, TRG ne
    const RawKit* spring_rugos =
        find_kit_by_barcode(need, QStringLiteral("ROL-RRG"));
    Q_ASSERT(spring_rugos != nullptr);

    const RawKit* spring_tetoteri =
        find_kit_by_barcode(need, QStringLiteral("ROL-TRG"));
    Q_ASSERT(spring_tetoteri == nullptr);
}

// 🧪 Tetőtéri roletta – ROL_RT
void RolettaCalculationTester::test_tetoteri_roletta(const TestRolettaBuilder::Ids& ids)
{
    OrderLine line;
    line.productId   = ids.ROL_RT;
    line.width_mm    = 1000;
    line.height_mm   = 1400;
    line.handlerSide = QStringLiteral("R");

    ItemNeed need = NeedCalculator::calculate(line, QStringLiteral("Manufacturing"), true);

    // Cutting: ugyanaz a logika, csak más méret
    const RawCut* shaft_cut =
        find_cut_by_barcode(need, QStringLiteral("TE-R-23"));
    Q_ASSERT(shaft_cut != nullptr);
    Q_ASSERT(shaft_cut->requiredLength == line.width_mm - 30);

    const RawCut* bottom_bar_cut =
        find_cut_by_barcode(need, QStringLiteral("ROL-P"));
    Q_ASSERT(bottom_bar_cut != nullptr);
    Q_ASSERT(bottom_bar_cut->requiredLength == line.width_mm - 30);

    const RawCut* fabric_cut =
        find_cut_by_barcode(need, QStringLiteral("ROL-FABRIC"));
    Q_ASSERT(fabric_cut != nullptr);
    Q_ASSERT(fabric_cut->requiredLength == line.height_mm - 20);

    // Kitting: tetőtéri rugó legyen, rugós rugó ne
    const RawKit* spring_tetoteri =
        find_kit_by_barcode(need, QStringLiteral("ROL-TRG"));
    Q_ASSERT(spring_tetoteri != nullptr);

    const RawKit* spring_rugos =
        find_kit_by_barcode(need, QStringLiteral("ROL-RRG"));
    Q_ASSERT(spring_rugos == nullptr);
}

// 🧪 Gyöngyös alap roletta – GY_A23
void RolettaCalculationTester::test_alap_roletta(const TestRolettaBuilder::Ids& ids)
{
    OrderLine line;
    line.productId  = ids.GY_A23;
    line.width_mm   = 800;
    line.height_mm  = 1200;
    line.handlerSide = QStringLiteral("L");

    ItemNeed need = NeedCalculator::calculate(line, QStringLiteral("Manufacturing"), true);

    // Cutting: tengely + alsópálca + vászon ugyanazzal a formulával
    const RawCut* shaft_cut =
        find_cut_by_barcode(need, QStringLiteral("TE-R-23"));
    Q_ASSERT(shaft_cut != nullptr);
    Q_ASSERT(shaft_cut->requiredLength == line.width_mm - 30);

    const RawCut* bottom_bar_cut =
        find_cut_by_barcode(need, QStringLiteral("ROL-P"));
    Q_ASSERT(bottom_bar_cut != nullptr);
    Q_ASSERT(bottom_bar_cut->requiredLength == line.width_mm - 30);

    const RawCut* fabric_cut =
        find_cut_by_barcode(need, QStringLiteral("ROL-FABRIC"));
    Q_ASSERT(fabric_cut != nullptr);
    Q_ASSERT(fabric_cut->requiredLength == line.height_mm - 20);

    // Kitting: gyöngyös záró legyen, rugók ne
    const RawKit* gyz_kit =
        find_kit_by_barcode(need, QStringLiteral("ROL-GYZ"));
    Q_ASSERT(gyz_kit != nullptr);

    const RawKit* spring_rugos =
        find_kit_by_barcode(need, QStringLiteral("ROL-RRG"));
    Q_ASSERT(spring_rugos == nullptr);

    const RawKit* spring_tetoteri =
        find_kit_by_barcode(need, QStringLiteral("ROL-TRG"));
    Q_ASSERT(spring_tetoteri == nullptr);
}
