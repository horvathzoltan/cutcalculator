#include "formula_contract.h"

// Cutting szerződés
FormulaContract cuttingContract()
{
    FormulaContract c;

    c.allowedInputs = {
        "w", "h", "qty",
        "handler", "owner", "color",
        "externalId", "product"
    };

    c.requiredOutputs = {
        "requiredLength",
        "qty"
    };

    c.optionalOutputs = {
        "material"
    };

    return c;
}

// Kitting szerződés
FormulaContract kittingContract()
{
    FormulaContract c;

    c.allowedInputs = {
        "w", "h", "qty",
        "handler", "owner", "color",
        "externalId", "product"
    };

    c.requiredOutputs = {
        "qty"
    };

    c.optionalOutputs = {
        "material"
    };

    return c;
}
