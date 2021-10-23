QT += core
QT += network
QT += websockets

HEADERS += \
    $$PWD/private/p_qrpc_http_headers.h \
    $$PWD/private/p_qrpc_http_response.h \
    $$PWD/private/p_qrpc_request_job_database.h \
    $$PWD/private/p_qrpc_request_job_wss.h \
    $$PWD/private/p_qrpc_util.h \
    $$PWD/private/p_qrpc_request.h \
    $$PWD/private/p_qrpc_listen_request_code.h \
    $$PWD/private/p_qrpc_request_job.h \
    $$PWD/private/p_qrpc_request_job_http.h \
    $$PWD/private/p_qrpc_request_job_protocol.h \
    $$PWD/private/p_qrpc_request_job_response.h \
    $$PWD/private/p_qrpc_request_job_tcp.h \
    $$PWD/private/p_qrpc_request_job_udp.h \
    $$PWD/private/p_qrpc_request_job_localsocket.h \
    $$PWD/qrpc_listen_request.h \
    $$PWD/qrpc_request_exchange.h \
    $$PWD/qrpc_request_exchange_setting.h \
    $$PWD/qrpc_request.h


SOURCES += \
    $$PWD/private/p_qrpc_http_headers.cpp \
    $$PWD/private/p_qrpc_http_response.cpp \
    $$PWD/private/p_qrpc_listen_request_code.cpp \
    $$PWD/qrpc_listen_request.cpp \
    $$PWD/qrpc_request_exchange.cpp \
    $$PWD/qrpc_request_exchange_setting.cpp \
    $$PWD/qrpc_request.cpp \
    $$PWD/private/p_qrpc_util.cpp \

INCLUDEPATH+=$$PWD
