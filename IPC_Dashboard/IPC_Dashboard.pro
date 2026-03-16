QT += quick quickcontrols2

CONFIG += c++17

TARGET = IPC_Dashboard
TEMPLATE = app

SOURCES += \
    main.cpp \
    vehicledata.cpp

HEADERS += \
    vehicledata.h

RESOURCES += resources.qrc

# Suppress Qt deprecation warnings from Qt 5.15+
DEFINES += QT_DEPRECATED_WARNINGS

# Raspberry Pi 4 cross-compile toolchain (uncomment when cross-compiling)
# QMAKE_CC  = arm-linux-gnueabihf-gcc
# QMAKE_CXX = arm-linux-gnueabihf-g++

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
