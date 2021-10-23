QT += core
QT += network
QT += websockets

HEADERS += \
    $$PWD/qrpc_interface_check.h \
    $$PWD/qrpc_interface_record.h \
    $$PWD/qrpc_interface_maintence.h

SOURCES += \
    $$PWD/qrpc_interface_check.cpp \
    $$PWD/qrpc_interface_record.cpp \
    $$PWD/qrpc_interface_maintence.cpp
