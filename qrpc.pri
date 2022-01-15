QT += core
QT += network
QT += websockets

include($$PWD/src/qrpc-base.pri)
include($$PWD/src/qrpc-token.pri)
include($$PWD/src/qrpc-client.pri)
include($$PWD/src/qrpc-server.pri)
include($$PWD/src/qrpc-interfaces.pri)


INCLUDEPATH+=$$PWD/includes

HEADERS += \
    $$PWD/includes/QRpc/Client \
    $$PWD/includes/QRpc/Server

RESOURCES += \
    $$PWD/src/qrpc.qrc
