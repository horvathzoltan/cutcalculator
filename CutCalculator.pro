QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

DEFINES += TARGI=$$TARGET
message( "TARGET = "$$TARGI )

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    colors/model/named_color.cpp \
    colors/registry/color_registry.cpp \
    colors/repository/color_repository.cpp \
    common/csv/filecontext.cpp \
    common/csv/filecontext_collector.cpp \
    common/logger/event_logger.cpp \
    common/logger/log_manager.cpp \
    common/logger/logger.cpp \
    common/registry/registry_base.cpp \
    common/registry/registry_manager.cpp \
    common/settings/settings_manager.cpp \
    common/startup/startup_manager.cpp \
    common/startup/startup_status_manager.cpp \
    common/system/lifecycle_manager.cpp \
    common/system/window_observer.cpp \
    common/utils/filehelper.cpp \
    common/utils/filename_helper.cpp \
    common/utils/project_root_locator.cpp \
    main.cpp \
    mainwindow.cpp \
    materials/view/material_table_widget.cpp \
    materials/model/crosssectionshape.cpp \
    materials/model/material_type.cpp \
    materials/registry/material_registry.cpp \
    materials/repository/material_repository.cpp \
    materials/view/material_table_manager.cpp \
    products/registry/product_registry.cpp \
    products/repository/product_repository.cpp \
    products/view/product_tree_manager.cpp \
    test/test_manager.cpp \
    ui/adapters/log_view_adapter.cpp

HEADERS += \
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
    test/eventlogger_tests.h \
    test/test_manager.h \
    ui/adapters/log_view_adapter.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md \
    run.txt \
    run_2.txt \
    run_3_1.txt \
    run_3_2.txt \
    run_4_AnyagszuksegletSzamitas.txt \
    run_4_ColorRegistry.txt \
    run_4_RegistryManager.txt \
    run_4_ThreePhaseImport.txt \
    run_4_ValidationRules.txt \
    run_4_WordcodeTranslation.txt \
    testdata/products.csv \
    testdata/settings.ini
