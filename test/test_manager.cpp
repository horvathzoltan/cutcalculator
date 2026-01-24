#include "test_manager.h"
#include <QDebug>

#include "eventlogger_tests.h"
#include "material/material_registry_smoke_test.h"
#include "filenamehelper_tests.h"
#include "product/product_registry_tester.h"
#include "test/calcmode/need_calculation_registry_tester.h"
#include "test/need/need_rule_registry_tester.h"


TestManager& TestManager::instance() {
    static TestManager inst;
    return inst;
}

bool TestManager::runBusinessLogicTests(const QString& profile) {
    _lastResults.clear();

    zInfo() << "▶️ Running business logic tests with profile:" << profile;

    // Példa: itt futtathatod a modulokhoz tartozó teszteket
    // Ezeket később bővítheted modulonként
    bool ok = true;

    if (profile == "eventlogger") {
        ok = runEventLoggerTests();
        _lastResults << (ok ? "EventLogger tests PASSED" : "EventLogger tests FAILED");
    }
    else if (profile == "material_smoke") {
        ok = runMaterialRegistrySmokeTest();
        _lastResults << (ok ? "MaterialRegistry smoke PASSED" : "MaterialRegistry smoke FAILED");
    }
    else if (profile == "default") {
        _lastResults << "Default tests executed";
    }
    else if (profile == "filenamehelper") {
        ok = runFileNameHelperTests();
        _lastResults << (ok ? "FileNameHelper tests PASSED" : "FileNameHelper tests FAILED");
    }
    else if (profile == "product") { // --test product
        ok = runProductRegistryTests();
        _lastResults << (ok ? "Product tests PASSED" : "Product tests FAILED");
    }
    else if (profile == "needrule") { // --test needrule
        ok = runNeedRuleRegistryTests();
        _lastResults << (ok ? "Product tests PASSED" : "Product tests FAILED");
    }
    else if (profile == "calcmode") { // --test needrule
        ok = runNeedCalculationRegistryTests();
        _lastResults << (ok ? "Product tests PASSED" : "Product tests FAILED");
    }
    /**/
    else {
        _lastResults << "Unknown profile:" + profile;
        ok = false;
    }

    zInfo() << "✅ Test results:" << _lastResults;
    return ok;
}

QStringList TestManager::lastResults() const {
    return _lastResults;
}
