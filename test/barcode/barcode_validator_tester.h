#pragma once
#include "test/common/test_module_base.h"

/* ============================================================
 * 🧩 BarcodeValidatorTester – Validator szabályok tesztelése
 * ============================================================ */
class BarcodeValidatorTester : public TestModuleBase {
public:
    BarcodeValidatorTester()
        : TestModuleBase("BarcodeValidator")
    {}

    bool run() override;

private:
    void testEmptyCodeRejected();
    void testInvalidCharactersRejected();
    void testDuplicateCodeRejected();
    void testDifferentEntityAllowed();
    void testRetiredCodeRejected();
    void testEntityIdRequired();
    void testPrefixRules();
    void testGeneratorIntegration();
    void testFallbackLogic();
};
