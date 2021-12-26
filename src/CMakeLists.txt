set(qrpc_SRCS
    # cmake-format: sortable
    3rdparty/qtwebapp/httpserver/httpconnectionhandler.cpp
    3rdparty/qtwebapp/httpserver/httpconnectionhandlerpool.cpp
    3rdparty/qtwebapp/httpserver/httpcookie.cpp
    3rdparty/qtwebapp/httpserver/httpglobal.cpp
    3rdparty/qtwebapp/httpserver/httplistener.cpp
    3rdparty/qtwebapp/httpserver/httprequest.cpp
    3rdparty/qtwebapp/httpserver/httprequesthandler.cpp
    3rdparty/qtwebapp/httpserver/httpresponse.cpp
    3rdparty/qtwebapp/httpserver/httpsession.cpp
    3rdparty/qtwebapp/httpserver/httpsessionstore.cpp
    3rdparty/qtwebapp/httpserver/staticfilecontroller.cpp
    #private/p_qrpc_amqp_handle.cpp
    #private/p_qrpc_amqp_pool.h
    #private/p_qrpc_amqp_thread.cpp
    #private/p_qrpc_amqp_thread_consumer.h
    #private/p_qrpc_amqp_thread_publish.h
    private/p_qrpc_controller_options.h
    private/p_qrpc_http_headers.cpp
    private/p_qrpc_http_response.cpp
    #private/p_qrpc_listen_broker_amqp.cpp
    private/p_qrpc_listen_broker_database.cpp
    private/p_qrpc_listen_broker_kafka.cpp
    #private/p_qrpc_listen_broker_mqtt.cpp
    private/p_qrpc_listen_http.cpp
    private/p_qrpc_listen_http_p.cpp
    private/p_qrpc_listen_local_socket.cpp
    private/p_qrpc_listen_qrpc.cpp
    private/p_qrpc_listen_qrpc_slot.cpp
    private/p_qrpc_listen_request_code.cpp
    private/p_qrpc_listen_tcp.cpp
    private/p_qrpc_listen_udp.cpp
    private/p_qrpc_listen_wss.cpp
    private/p_qrpc_request.h
    private/p_qrpc_request_job.h
    #private/p_qrpc_request_job_amqp.cpp
    private/p_qrpc_request_job_database.h
    private/p_qrpc_request_job_http.h
    private/p_qrpc_request_job_localsocket.h
    private/p_qrpc_request_job_protocol.h
    private/p_qrpc_request_job_response.h
    private/p_qrpc_request_job_tcp.h
    private/p_qrpc_request_job_udp.h
    private/p_qrpc_request_job_wss.h
    private/p_qrpc_server.h
    private/p_qrpc_thread.cpp
    private/p_qrpc_thread_ping.cpp
    private/p_qrpc_util.cpp
    qrpc_connection_setting.h
    qrpc_const.h
    qrpc_controller.cpp
    qrpc_controller_options.cpp
    qrpc_controller_router.cpp
    qrpc_controller_setting.h
    qrpc_global.h
    qrpc_interface_check.cpp
    qrpc_interface_maintence.cpp
    qrpc_interface_record.cpp
    qrpc_last_error.cpp
    qrpc_listen.cpp
    qrpc_listen_colletion.cpp
    qrpc_listen_protocol.cpp
    qrpc_listen_request.cpp
    qrpc_listen_request_cache.cpp
    qrpc_listen_request_parser.cpp
    qrpc_macro.h
    qrpc_request.cpp
    qrpc_request_exchange.cpp
    qrpc_request_exchange_setting.cpp
    qrpc_server.cpp
    qrpc_service_manager.h
    qrpc_service_setting.cpp
    qrpc_service_thread.cpp
    qrpc_token_pool.cpp
    qrpc_types.h
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    add_library(qrpc STATIC ${qrpc_SRCS})
else()
    add_library(qrpc SHARED ${qrpc_SRCS})
endif()

target_link_libraries(
    qrpc # cmake-format: sortable
)

install(TARGETS qrpc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt/plugins/qrpc
)
