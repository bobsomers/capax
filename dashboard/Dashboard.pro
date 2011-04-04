# -------------------------------------------------
# Project created by QtCreator 2010-01-14T05:25:26
# -------------------------------------------------
TARGET = Dashboard
TEMPLATE = app
SOURCES += main.cpp \
    TestHarness.cpp \
    DataLogger.cpp \
    Dashboard.cpp \
    Hardware.cpp \
    Options.cpp \
    qneedleindicator.cpp
HEADERS += TestHarness.h \
    ucvtypes.h \
    DataLogger.h \
    Dashboard.h \
    Hardware.h \
    Options.h \
    main.h \
    qneedleindicator.h
LIBS += -lws2_32

CONFIG(release, debug|release) {
    DEFINES *= RUNNING_IN_CAR
}
