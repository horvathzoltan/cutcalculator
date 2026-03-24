#pragma once
#include "test/common/test_module_base.h"

/* ============================================================
 * 🧩 BarcodeGeneratorTester – slug + token + generate tesztek
 * ============================================================ */
class BarcodeGeneratorTester : public TestModuleBase {
public:
    BarcodeGeneratorTester()
        : TestModuleBase("BarcodeGenerator")
    {}

    bool run() override;

private:
    void testSlugBasic();
    void testSlugUnicode();
    void testTokenLength();
    void testGeneratePrefixAndLength();
    void testGenerateStability();
    void testGenerateUniqueness();
};
