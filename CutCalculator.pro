QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

DEFINES += TARGI=$$TARGET
message( "TARGET = "$$TARGI )

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/color/namedcolor.cpp \
    common/color/ral_importer.cpp \
    common/csv/filecontext_collector.cpp \
    common/logger/event_logger.cpp \
    common/logger/log_manager.cpp \
    common/logger/logger.cpp \
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
    materials/model/crosssectionshape.cpp \
    materials/model/material_type.cpp \
    materials/registry/material_registry.cpp \
    materials/repository/material_repository.cpp \
    test/test_manager.cpp \
    ui/adapters/log_view_adapter.cpp

HEADERS += \
    common/color/colorconstants.h \
    common/color/namedcolor.h \
    common/color/ral_importer.h \
    common/color/ralsystem.h \
    common/csv/csvhelper.h \
    common/csv/csvimporter.h \
    common/csv/filecontext_collector.h \
    common/logger/event_logger.h \
    common/logger/log_manager.h \
    common/logger/logger.h \
    common/model/hierarchical_entity.h \
    common/model/identifiable_entity.h \
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
    mainwindow.h \
    materials/model/crosssectionshape.h \
    materials/model/cutting_mode.h \
    materials/model/material_master.h \
    materials/model/material_type.h \
    materials/model/painting_mode.h \
    materials/registry/material_registry.h \
    materials/repository/material_repository.h \
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
    run_2.txt \
    run_3.txt \
    testdata/settings.ini
