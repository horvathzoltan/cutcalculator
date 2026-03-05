#include "test_manager.h"
#include <QDebug>

#include "material/material_registry_tester.h"
#include "product/product_registry_tester.h"
#include "settings_manager_tester.h"
#include "test/calcmode/need_calculation_registry_tester.h"
#include "test/calculation/formula_engine_pipeline_tester.h"
#include "test/calculation/formula_engine_tester.h"
#include "test/calculation/need_calculation_detail_registry_tester.h"

#include "test/calculation/need_calculator_pipeline_tester.h"
#include "test/eventlogger/event_logger_tester.h"
#include "test/filenamehelper/filenamehelper_tests.h"
#include "test/need/need_rule_registry_tester.h"
#include "test/overlay/material_requirements_overlay_tester.h"
#include "test/overlay/matrix_validator_tester.h"
#include "test/overlay/overlay_presenter_tester.h"

TestManager& TestManager::instance() {
    static TestManager inst;
    return inst;
}

void TestManager::runBusinessLogicTests(const QString& profile) {
    _lastResults.clear();

    zInfo() << "▶️ Running business logic tests with profile:" << profile;

    // Példa: itt futtathatod a modulokhoz tartozó teszteket
    // Ezeket később bővítheted modulonként

    if (profile == "default") {
        _lastResults << "Default tests executed";
    }
    else if (profile == "eventlogger") {
        runModule<EventLoggerTester>();
    }
    else if (profile == "filename") {
        runModule<FileNameHelperTester>();
    }
    else if (profile == "material") {
        runModule<MaterialRegistryTester>();
    }
    else if (profile == "product") {
        runModule<ProductRegistryTester>();
    }
    else if (profile == "needrule") {
        runModule<NeedRuleRegistryTester>();
    }
    else if (profile == "calcmode") { // --test needrule
        runModule<NeedCalculationRegistryTester>();
    }
    else if (profile == "need_calculation_detail") {
        runModule<NeedCalculationDetailRegistryTester>();
    }
    else if (profile == "overlay") {
        runModule<OverlayPresenterTester>();
    }
    else if (profile == "materialrequirement_overlay") {
        runModule<MaterialRequirementsOverlayTester>();
    }
    else if (profile == "matrixvalidator") {
        runModule<MatrixValidatorTester>();
    }
    else if (profile == "settings") {
        runModule<SettingsManagerTester>();
    }
    else if (profile == "formula_engine") {
        runModule<FormulaEngineTester>();
    }
    else if (profile == "formula_engine_pipeline") {
        runModule<FormulaEnginePipelineTester>();
    }
    else if (profile == "need_calculation_pipeline") {
        runModule<NeedCalculatorPipelineTester>();
    }
    /**/
    else {
        _lastResults << "Unknown profile:" + profile;
    }

    zInfo() << "✅ Test results:" << _lastResults;
}

QStringList TestManager::lastResults() const {
    return _lastResults;
}
