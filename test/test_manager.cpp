#include "test_manager.h"
#include <QDebug>

#include "material/material_registry_tester.h"
#include "product/product_registry_tester.h"
#include "settings_manager_tester.h"
#include "test/calcmode/need_calculation_registry_tester.h"
#include "test/calculation/formula_engine_pipeline_tester.h"
#include "test/calculation/formula_engine_syntax_tester_2.h"
#include "test/calculation/formula_engine_tester.h"
#include "test/calculation/need_calculation_detail_registry_tester.h"

#include "test/calculation/need_calculator_pipeline_tester.h"
#include "test/calculation/roletta_calculation_tester.h"
#include "test/calculation/test_roletta_builder_tester.h"
#include "test/eventlogger/event_logger_tester.h"
#include "test/filenamehelper/filenamehelper_tests.h"
#include "test/need/need_rule_registry_tester.h"
#include "test/overlay/material_requirements_overlay_tester.h"
#include "test/overlay/matrix_validator_tester.h"
#include "test/overlay/overlay_presenter_tester.h"
#include <test/calculation/formula_engine_syntax_tester.h>

TestManager& TestManager::instance() {
    static TestManager inst;
    return inst;
}

void TestManager::runBusinessLogicTests(const QString& profile) {
    _lastResults.clear();

    zInfo() << "▶️ Running business logic tests with profile:" << profile;

    // --- Alap tesztek ---
    if (profile == "default") {
        _lastResults << "Default tests executed";
    }
    else if (profile == "eventlogger") {
        runModule<EventLoggerTester>();
    }
    else if (profile == "filename") {
        runModule<FileNameHelperTester>();
    }
    else if (profile == "settings") {
        runModule<SettingsManagerTester>();
    }

    // --- Registry tesztek ---
    else if (profile == "material") {
        runModule<MaterialRegistryTester>();
    }
    else if (profile == "product") {
        runModule<ProductRegistryTester>();
    }
    else if (profile == "needrule") {
        runModule<NeedRuleRegistryTester>();
    }
    else if (profile == "calcmode") {
        runModule<NeedCalculationRegistryTester>();
    }
    else if (profile == "need_calculation_detail") {
        runModule<NeedCalculationDetailRegistryTester>();
    }

    // --- Formula engine tesztek ---
    else if (profile == "formula_engine") {
        runModule<FormulaEngineTester>();
    }
    else if (profile == "formula_engine_pipeline") {
        runModule<FormulaEnginePipelineTester>();
    }
    else if (profile == "formula_engine_syntax") {
        runModule<FormulaEngineSyntaxTester>();
    }
    else if (profile == "formula_engine_syntax_2") {
        runModule<FormulaEngineSyntaxTester_2>();
    }

    // --- NeedCalculator pipeline ---
    else if (profile == "need_calculation_pipeline") {
        runModule<NeedCalculatorPipelineTester>();
    }

    // --- Roletta-specifikus tesztek ---
    else if (profile == "roletta_builder") {
        runModule<TestRolettaBuilderTester>();
    }
    else if (profile == "roletta_calc") {
        runModule<RolettaCalculationTester>();
    }

    // --- Overlay / GUI logika ---
    else if (profile == "overlay") {
        runModule<OverlayPresenterTester>();
    }
    else if (profile == "materialrequirement_overlay") {
        runModule<MaterialRequirementsOverlayTester>();
    }
    else if (profile == "matrixvalidator") {
        runModule<MatrixValidatorTester>();
    }

    // --- Ismeretlen profil ---
    else {
        _lastResults << "Unknown profile:" + profile;
    }

    zInfo() << "✅ Test results:" << _lastResults;
}


QStringList TestManager::lastResults() const {
    return _lastResults;
}
