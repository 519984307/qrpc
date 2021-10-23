QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

include($$PWD/src/qrpc-base.pri)
include($$PWD/src/qrpc-token.pri)
include($$PWD/src/qrpc-client.pri)
include($$PWD/src/qrpc-server.pri)
include($$PWD/src/qrpc-interfaces.pri)

RESOURCES += \
    $$PWD/src/qrpc.qrc
