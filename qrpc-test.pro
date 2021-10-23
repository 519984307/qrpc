QT += testlib

CONFIG += testcase
CONFIG += console
CONFIG += silent
CONFIG -= debug_and_release
QT -= gui

TEMPLATE = app
TARGET = QRpcTest

INCLUDEPATH+=$$PWD/../src
#INCLUDEPATH+=$$PWD/../src/private

QRPC_TEST_MODE=true
QMAKE_CXXFLAGS += -DQRPC_TEST_MODE=\\\"$$QRPC_TEST_MODE\\\"

LIBS += -lgmock
LIBS += -lgtest

include($$PWD/qrpc.pri)
include($$PWD/test/qrpc-test.pri)

