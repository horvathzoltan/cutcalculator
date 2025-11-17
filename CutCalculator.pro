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
    common/logger/event_logger.cpp \
    common/logger/logger.cpp \
    common/settings/settings_manager.cpp \
    common/utils/filename_helper.cpp \
    common/utils/project_root_locator.cpp \
    main.cpp \
    mainwindow.cpp \
    materials/model/crosssectionshape.cpp \
    materials/model/material_type.cpp \
    test/test_manager.cpp \
    ui/adapters/log_view_adapter.cpp

HEADERS += \
    common/color/colorconstants.h \
    common/color/namedcolor.h \
    common/logger/event_logger.h \
    common/logger/logger.h \
    common/model/hierarchical_entity.h \
    common/model/identifiable_entity.h \
    common/settings/settings_manager.h \
    common/system/buildnumber.h \
    common/system/signal_helper.h \
    common/system/sysinfo_helper.h \
    common/utils/filename_helper.h \
    common/utils/project_root_locator.h \
    mainwindow.h \
    materials/model/crosssectionshape.h \
    materials/model/cutting_mode.h \
    materials/model/material_master.h \
    materials/model/material_type.h \
    materials/model/painting_mode.h \
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
    run_3.txt
