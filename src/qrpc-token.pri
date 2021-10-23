QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

HEADERS += \
    $$PWD/qrpc_token_pool.h

SOURCES += \
    $$PWD/qrpc_token_pool.cpp
