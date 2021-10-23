QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

QT_MESSAGE_PATTERN="[%{type}] %{appname} (%{file}:%{line}) - %{message}"

include($$PWD/qrpc-base.pri)
include($$PWD/qrpc-client.pri)

RESOURCES += \
    $$PWD/qrpc.qrc
