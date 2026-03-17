#include "test_roletta_builder_tester.h"

#include "test/calculation/test_roletta_builder.h"

#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"

namespace {

bool has_detail_kind(const QVector<NeedCalculationDetail>& details,
                     NeedCalculationDetail::DetailKind kind)
{
    for (const auto& d : details)
        if (d.kind == kind)
            return true;
    return false;
}

} // namespace

bool TestRolettaBuilderTester::run()
{
    auto ids = TestRolettaBuilder::build();

    auto& product_reg = ProductRegistry::instance();
    auto& material_reg = MaterialRegistry::instance();
    auto& rule_reg = NeedRuleRegistry::instance();
    auto& calc_reg = NeedCalculationRegistry::instance();
    auto& detail_reg = NeedCalculationDetailRegistry::instance();

    // --- Products ---
    Q_ASSERT(product_reg.findById(ids.ROL_R));
    Q_ASSERT(product_reg.findById(ids.ROL_RT));
    Q_ASSERT(product_reg.findById(ids.GY_A23));

    // --- Materials ---
    Q_ASSERT(material_reg.findById(ids.TE_R_23));
    Q_ASSERT(material_reg.findById(ids.ROL_P));
    Q_ASSERT(material_reg.findById(ids.ROL_RRG));
    Q_ASSERT(material_reg.findById(ids.ROL_TRG));
    Q_ASSERT(material_reg.findById(ids.ROL_GYZ));
    Q_ASSERT(material_reg.findById(ids.ROL_ENDCAP));
    Q_ASSERT(material_reg.findById(ids.ROL_P_ENDCAP));
    Q_ASSERT(material_reg.findById(ids.ROL_FABRIC));

    // --- BOM rules ---
    Q_ASSERT(!rule_reg.findByLeft(ids.ROL_R).isEmpty());
    Q_ASSERT(!rule_reg.findByLeft(ids.ROL_RT).isEmpty());
    Q_ASSERT(!rule_reg.findByLeft(ids.GY_A23).isEmpty());

    auto check_calc = [&](const QUuid& calcId, const QString& expectedName) {
        const NeedCalculation* calc = calc_reg.findById(calcId);
        Q_ASSERT(calc);
        Q_ASSERT(calc->name == expectedName);

        auto details = detail_reg.findByCalculation(calcId);
        Q_ASSERT(!details.isEmpty());
        Q_ASSERT(has_detail_kind(details, NeedCalculationDetail::DetailKind::Cutting));
        Q_ASSERT(has_detail_kind(details, NeedCalculationDetail::DetailKind::Kitting));
    };

    // --- Manufacturing modes ---
    check_calc(ids.calcManufacturingRolR,  "Manufacturing");
    check_calc(ids.calcManufacturingRolRT, "Manufacturing");
    check_calc(ids.calcManufacturingGyA23, "Manufacturing");

    return true;
}
