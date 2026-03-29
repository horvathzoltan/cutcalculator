#include "formula_contract.h"

// Cutting szerződés
// material output STRING (barcode), nem numeric
FormulaContract cuttingContract()
{

    FormulaContract c;

    c.allowedInputs = {
        "w", "h", "qty",
        "handler", "owner", "color",
        "externalId", "product"
    };

    c.requiredOutputs = {
        "len"
    };

    c.optionalOutputs = {
        "mat",
        "qty"   // Cutting esetén opcionális
    };

    return c;
}


// Kitting szerződés
// material output STRING (barcode), nem numeric
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
