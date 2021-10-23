#pragma once

#define Q_RPC_LOG true
#define Q_RPC_LOG_VERBOSE false
#define Q_RPC_LOG_SUPER_VERBOSE false

#if Q_RPC_LOG_VERBOSE
    #undef Q_RPC_LOG
    #define Q_RPC_LOG true
#endif

#if Q_RPC_LOG_SUPER_VERBOSE
    #undef Q_RPC_LOG
    #undef Q_RPC_LOG_VERBOSE

    #define Q_RPC_LOG true
    #define Q_RPC_LOG_VERBOSE true
#endif


#if Q_RPC_LOG
    #ifdef QT_DEBUG
        #define Q_RPC_LOG_DEBUG true
    #else
        #define Q_RPC_LOG_DEBUG false
    #endif

    #ifdef QT_RELEASE
        #define Q_RPC_LOG_RELEASE true
    #else
        #define Q_RPC_LOG_RELEASE false
    #endif
#else
    #define Q_RPC_LOG_DEBUG false
    #define Q_RPC_LOG_RELEASE false
#endif
