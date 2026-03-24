#pragma once
#include "test/common/test_module_base.h"

/* ============================================================
 * 🧩 BarcodeRegistryTester – ledger viselkedés tesztjei
 * ============================================================ */
class BarcodeRegistryTester : public TestModuleBase {
public:
    BarcodeRegistryTester()
        : TestModuleBase("BarcodeRegistry")
    {}

    bool run() override;

private:
    void testRegisterNewUnique();
    void testRegisterCollisionDifferentEntity();
    void testRegisterSameEntityTwice();
    void testRetire();
    void testRetireThenRegisterAgain();
    void testEntityIdMissingNoCollision();
    void testCsvRoundtrip();
};
