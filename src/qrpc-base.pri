QT += core
QT += network
QT += websockets

INCLUDEPATH+=$$PWD

HEADERS += \
    $$PWD/private/p_qrpc_controller_options.h \
#    $$PWD/private/p_qrpc_thread_ping.h \
    $$PWD/qrpc_const.h \
    $$PWD/qrpc_controller_options.h \
    $$PWD/qrpc_controller_setting.h \
    $$PWD/qrpc_global.h \
    $$PWD/qrpc_last_error.h \
    $$PWD/qrpc_macro.h \
    $$PWD/qrpc_service_manager.h \
    $$PWD/qrpc_service_setting.h \
    $$PWD/qrpc_service_thread.h \
    $$PWD/qrpc_types.h \

SOURCES += \
#    $$PWD/private/p_qrpc_thread_ping.cpp \
    $$PWD/qrpc_service_setting.cpp \
    $$PWD/qrpc_controller_options.cpp \
    $$PWD/qrpc_last_error.cpp \
    $$PWD/qrpc_service_thread.cpp

