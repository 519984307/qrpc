QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

include($$PWD/3rdparty/qtwebapp/qtwebapp.pri)

HEADERS += \
    $$PWD/private/p_qrpc_listen_broker_amqp.h \
    $$PWD/private/p_qrpc_listen_broker_database.h \
    $$PWD/private/p_qrpc_listen_broker_kafka.h \
    $$PWD/private/p_qrpc_listen_broker_mqtt.h \
    $$PWD/private/p_qrpc_listen_http.h \
    $$PWD/private/p_qrpc_listen_local_socket.h \
    $$PWD/private/p_qrpc_listen_qrpc.h \
    $$PWD/private/p_qrpc_listen_qrpc_slot.h \
    $$PWD/private/p_qrpc_listen_tcp.h \
    $$PWD/private/p_qrpc_listen_udp.h \
    $$PWD/private/p_qrpc_listen_wss.h \
    $$PWD/private/p_qrpc_server.h \
    $$PWD/private/p_qrpc_thread.h \
    $$PWD/qrpc_controller.h \
    $$PWD/qrpc_controller_router.h \
    $$PWD/qrpc_listen.h \
    $$PWD/qrpc_listen_request_parser.h \
    $$PWD/qrpc_listen_colletion.h \
    $$PWD/qrpc_listen_protocol.h \
    $$PWD/qrpc_listen_request_cache.h \
    $$PWD/qrpc_server.h \

SOURCES += \
    $$PWD/private/p_qrpc_listen_broker_amqp.cpp \
    $$PWD/private/p_qrpc_listen_broker_database.cpp \
    $$PWD/private/p_qrpc_listen_broker_kafka.cpp \
    $$PWD/private/p_qrpc_listen_broker_mqtt.cpp \
    $$PWD/private/p_qrpc_listen_http.cpp \
    $$PWD/private/p_qrpc_listen_local_socket.cpp \
    $$PWD/private/p_qrpc_listen_qrpc.cpp \
    $$PWD/private/p_qrpc_listen_qrpc_slot.cpp \
    $$PWD/private/p_qrpc_listen_tcp.cpp \
    $$PWD/private/p_qrpc_listen_udp.cpp \
    $$PWD/private/p_qrpc_listen_wss.cpp \
    $$PWD/private/p_qrpc_thread.cpp \
    $$PWD/qrpc_controller.cpp \
    $$PWD/qrpc_controller_router.cpp \
    $$PWD/qrpc_listen.cpp \
    $$PWD/qrpc_listen_request_parser.cpp \
    $$PWD/qrpc_listen_colletion.cpp \
    $$PWD/qrpc_listen_protocol.cpp \
    $$PWD/qrpc_listen_request_cache.cpp \
    $$PWD/qrpc_server.cpp \
