#pragma once

#include "products/registry/product_registry.h"
#include "materials/registry/material_registry.h"
#include "needs/registry/need_rule_registry.h"
#include "calcmodes/registry/need_calculation_registry.h"
#include "calculation/registry/need_calculation_detail_registry.h"

class TestRolettaBuilder {
public:
    struct Ids {
        // Products
        QUuid ROL_R;     // Rugós
        QUuid ROL_RT;    // Tetőtéri
        QUuid GY_A23;    // Alap gyöngyös

        // Materials
        QUuid TE_R_23;       // Tengely
        QUuid ROL_P;         // Alsópálca
        QUuid ROL_RRG;       // Rugós rugó
        QUuid ROL_TRG;       // Tetőtéri rugó
        QUuid ROL_GYZ;       // Gyöngyös záró
        QUuid ROL_ENDCAP;    // Tengelyvég dugó
        QUuid ROL_P_ENDCAP;  // Alsópálca dugó
        QUuid ROL_FABRIC;    // Vászon

        // Calculation modes – mindhárom termékhez külön Manufacturing mód
        QUuid calcManufacturingRolR;
        QUuid calcManufacturingRolRT;
        QUuid calcManufacturingGyA23;
    };

    static Ids build();
};
