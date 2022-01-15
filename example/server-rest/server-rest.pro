QT -= gui

CONFIG += c++17 console silent
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/../../../qstm/qstm.pri)
include($$PWD/../../qrpc.pri)

SOURCES += \
        controllerMethods.cpp \
        main.cpp

RESOURCES += \
    settings.qrc

HEADERS += \
    controllerMethods.h
