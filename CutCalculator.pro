QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

DEFINES += TARGI=$$TARGET
message( "TARGET = "$$TARGI )

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    barcodes/helpers/barcode_collision_helper.cpp \
    barcodes/registry/barcode_registry.cpp \
    barcodes/repository/barcode_repository.cpp \
    barcodes/validator/barcode_validator.cpp \
    calcmodes/dialogs/mode_name_dialog.cpp \
    common/layout/layout_default_store.cpp \
    common/snapshot/snapshot_manager.cpp \
    common/system/verbose_manager.cpp \
    common/system/verbose_registry_initializer.cpp \
    common/utils/geometry_helper.cpp \
    common/utils/table_formatter.cpp \
    mainwindow.cpp \
    materials/view/color_badge.cpp \
    needs/manager/material_requirements_manager.cpp \
    needs/registry/need_rule_registry.cpp \
    needs/view/material_picker_dialog.cpp \
    needs/view/material_requirements_view.cpp \
    calculation/manager/calculation_mode_detail_manager.cpp \
    calcmodes/manager/calculation_modes_manager.cpp \
    calculation/registry/need_calculation_detail_registry.cpp \
    calcmodes/registry/need_calculation_registry.cpp \
    calculation/repository/need_calculation_detail_repository.cpp \
    calcmodes/repository/need_calculation_repository.cpp \
    calculation/service/need_calculator.cpp \
    calculation/view/calculation_mode_detail_view.cpp \
    calcmodes/view/calculation_modes_view.cpp \
    workbench/view/bom_workbench.cpp \
    colors/model/named_color.cpp \
    colors/registry/color_registry.cpp \
    colors/repository/color_repository.cpp \
    common/csv/filecontext.cpp \
    common/csv/filecontext_collector.cpp \
    common/logger/event_logger.cpp \
    common/logger/log_manager.cpp \
    common/logger/logger.cpp \
    common/registry/registry_base.cpp \
    common/settings/settings_manager.cpp \
    common/startup/startup_manager.cpp \
    common/startup/startup_status_manager.cpp \
    common/system/lifecycle_manager.cpp \
    common/system/window_observer.cpp \
    common/utils/filehelper.cpp \
    common/utils/filename_helper.cpp \
    common/utils/project_root_locator.cpp \
    main.cpp \
    materials/view/material_table_widget.cpp \
    materials/model/crosssectionshape.cpp \
    materials/model/material_type.cpp \
    materials/registry/material_registry.cpp \
    materials/repository/material_repository.cpp \
    materials/view/material_table_manager.cpp \
    products/registry/product_registry.cpp \
    products/repository/product_repository.cpp \
    products/view/product_tree_manager.cpp \
    products/view/product_tree_view.cpp \
    test/test_manager.cpp \
    ui/adapters/log_view_adapter.cpp

HEADERS += \
    barcodes/model/barcode_record.h \
    barcodes/registry/barcode_registry.h \
    barcodes/repository/barcode_repository.h \
    barcodes/helpers/barcode_collision_helper.h \
    barcodes/validator/barcode_validator.h \
    calcmodes/dialogs/mode_name_dialog.h \
    common/layout/layout_default_store.h \
    common/registry/registry_lookup.h \
    common/registry/registry_traits.h \
    common/snapshot/snapshot_manager.h \
    common/snapshot/workbench_snapshot.h \
    common/system/nameof.hpp \
    common/system/verbose_manager.h \
    common/utils/font_utils.h \
    common/utils/geometry_helper.h \
    common/utils/optional_utils.h \
    common/utils/table_formatter.h \
    connections/connection_entity.h \
    connections/connection_registry.h \
    connections/connection_repository.h \
    materials/view/color_badge.h \
    needs/manager/material_requirements_manager.h \
    needs/model/need_rule.h \
    needs/presenter/need_rule_presenter.h \
    needs/registry/need_rule_registry.h \
    needs/repository/need_rule_repository.h \
    needs/repository/need_rule_traits.h \
    needs/view/material_picker_dialog.h \
    needs/view/material_requirements_view.h \
    calculation/manager/calculation_mode_detail_manager.h \
    calcmodes/manager/calculation_modes_manager.h \
    calcmodes/model/need_calculation.h \
    calculation/model/need_calculation_detail.h \
    calculation/registry/need_calculation_detail_registry.h \
    calcmodes/registry/need_calculation_registry.h \
    calculation/repository/need_calculation_detail_repository.h \
    calcmodes/repository/need_calculation_repository.h \
    calculation/service/need_calculator.h \
    calculation/view/calculation_mode_detail_view.h \
    calcmodes/view/calculation_modes_view.h \
    workbench/view/bom_workbench.h \
    colors/model/named_color.h \
    colors/registry/color_registry.h \
    colors/model/colorconstants.h \
    colors/model/ralsystem.h \
    colors/repository/color_repository.h \
    common/csv/csvhelper.h \
    common/csv/csvimporter.h \
    common/csv/filecontext_collector.h \
    common/logger/error_bucketizer.h \
    common/logger/event_logger.h \
    common/csv/filecontext.h \
    common/logger/log_manager.h \
    common/logger/logger.h \
    common/csv/rowerror.h \
    common/model/hierarchical_entity.h \
    common/model/identifiable_entity.h \
    common/registry/registry_base.h \
    common/registry/registry_manager.h \
    common/settings/settings_manager.h \
    common/startup/startup_manager.h \
    common/startup/startup_status.h \
    common/startup/startup_status_manager.h \
    common/system/buildnumber.h \
    common/system/lifecycle_manager.h \
    common/system/signal_helper.h \
    common/system/sysinfo_helper.h \
    common/system/window_observer.h \
    common/utils/filehelper.h \
    common/utils/filename_helper.h \
    common/utils/project_root_locator.h \
    common/utils/qt_event_util.h \
    common/utils/scoped_per_thread_lock.h \
    mainwindow.h \
    materials/view/material_table_widget.h \
    materials/model/crosssectionshape.h \
    materials/model/cutting_mode.h \
    materials/model/material_master.h \
    materials/model/material_type.h \
    materials/model/painting_mode.h \
    materials/registry/material_registry.h \
    materials/repository/material_repository.h \
    materials/view/material_table_manager.h \
    products/model/product_master.h \
    products/registry/product_registry.h \
    products/repository/product_repository.h \
    products/view/product_tree_manager.h \
    products/view/product_tree_view.h \
    test/eventlogger_tests.h \
    test/test_manager.h \
    ui/adapters/log_view_adapter.h \
    ui/style/color_helper.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md \
    run.txt \
    run_2_1_Kodgeneralas.txt \
    run_2_2_Dokumentacio.txt \
    run_2_3_Allapottabla.txt \
    run_3_1_Manifest.txt \
    run_3_2_Prompt.txt \
    run_4_1_Rendszerterv.txt \
    run_4_AnyagszuksegletSzamitas.txt \
    run_4_BOMWorkbench.txt \
    run_4_ColorRegistry.txt \
    run_4_GenerikusKapcsolotabla.txt \
    run_4_LogDoc.txt \
    run_4_RegistryManager.txt \
    run_4_ThreePhaseImport.txt \
    run_4_ValidationRules.txt \
    run_4_WidgetGeometrySettings.txt \
    run_4_WordcodeTranslation.txt \
    run_5_0.md \
    run_5_1.md \
    testdata/barcodes.csv \
    testdata/products.csv \
    testdata/needrules.csv \
    testdata/settings.ini
