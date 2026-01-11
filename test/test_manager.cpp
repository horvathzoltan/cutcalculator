#include "test_manager.h"
#include <QDebug>

#include "eventlogger_tests.h"
#include "material_registry_smoke_test.h"

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
