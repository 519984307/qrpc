include($$PWD/../qrpc.pri)

INCLUDEPATH+=$$PWD

CONFIG += console
CONFIG -= debug_and_release
CONFIG += testcase
LIBS += -L/usr/local/lib -lgmock
LIBS += -L/usr/local/lib -lgtest

Q_RPC_TEST=true
QMAKE_CXXFLAGS += -DQ_RPC_TEST=\\\"$$Q_RPC_TEST\\\"

HEADERS += \
    $$PWD/qrpc_test.h \
    $$PWD/qrpc_test_unit.h \
    $$PWD/qrpc_test_functional.h

SOURCES += \
    $$PWD/functional_test_request_json.cpp \
    $$PWD/functional_test_request_http.cpp \
    $$PWD/functional_test_request_xml.cpp \
    $$PWD/functional_test_request_cbor.cpp \
    $$PWD/functional_test_request_cors.cpp \
    $$PWD/functional_test_upload_download.cpp \
    $$PWD/functional_test_request_broker_database.cpp

