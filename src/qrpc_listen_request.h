#pragma once

#include "./qrpc_global.h"
#include "./private/p_qrpc_http_headers.h"
#include "./private/p_qrpc_listen_request_code.h"
#include <QThread>
#include <QTemporaryFile>
#include <QVariantHash>
#include <QByteArray>
#include <QUuid>
#include "./qstm_util_variant.h"
#include "./qrpc_controller_setting.h"

namespace QRpc {

    class QRPCRequest;
    class Q_RPC_EXPORT QRPCListenRequest: public QObject
    {
        Q_OBJECT
        friend class QRPCServer;
        friend class QRPCListenRequestCache;
        friend class QRPCListenQRPCSlotPvt;
        friend class QRPCRequestPvt;
    public:

        Q_PROPERTY(QUuid        listenUuid              READ listenUuid         WRITE setListenUuid          )
        Q_PROPERTY(int          requestContentType      READ requestContentType WRITE setRequestContentType  )
        Q_PROPERTY(QUuid        requestUuid             READ requestUuid        WRITE setRequestUuid         )
        Q_PROPERTY(QByteArray   requestProtocol         READ requestProtocol    WRITE setRequestProtocol     )
        Q_PROPERTY(QByteArray   requestPath             READ requestPath        WRITE setRequestPath         )
        Q_PROPERTY(QByteArray   requestMethod           READ requestMethod      WRITE setRequestMethod       )
        Q_PROPERTY(QVariantHash requestHeader           READ requestHeader      WRITE setRequestHeader       )
        Q_PROPERTY(QVariantHash requestParameter        READ requestParameter   WRITE setRequestParameter    )
        Q_PROPERTY(QVariant     requestBody             READ requestBody        WRITE setRequestBody         )
        Q_PROPERTY(int          requestTimeout          READ requestTimeout     WRITE setRequestTimeout      )
        Q_PROPERTY(QVariantHash responseHeader          READ responseHeader     WRITE setResponseHeader      )
        Q_PROPERTY(QVariant     responseBody            READ responseBody       WRITE setResponseBody        )
        Q_PROPERTY(int          responseCode            READ responseCode       WRITE setResponseCode        )
        Q_PROPERTY(QByteArray   responsePhrase          READ responsePhrase     WRITE setResponsePhrase      )
        Q_PROPERTY(QVariantHash responseCallback        READ responseCallback   WRITE setResponseCallback    )

    public:
        Q_INVOKABLE explicit QRPCListenRequest(QObject *parent=nullptr);
        //Q_INVOKABLE explicit QRPCListenRequest(const QRPCListenRequest &request, QObject *parent=nullptr);
        Q_INVOKABLE explicit QRPCListenRequest(const QVariant &requestBody, QObject *parent=nullptr);
        explicit QRPCListenRequest(const QVariant &requestBody, const ControllerSetting &setting, QObject *parent=nullptr);
        Q_INVOKABLE virtual ~QRPCListenRequest();
    public:
        /**
         * @brief vu
         */
        VariantUtil vu;
    public:

        QByteArray authorizationBasic() const;
        QByteArray authorizationBearer() const;
        QByteArray authorizationService() const;

        Q_INVOKABLE bool isMethodHead() const;
        Q_INVOKABLE bool isMethodGet() const;
        Q_INVOKABLE bool isMethodPost() const;
        Q_INVOKABLE bool isMethodPut() const;
        Q_INVOKABLE bool isMethodDelete() const;
        Q_INVOKABLE bool isMethodUpload() const;
        Q_INVOKABLE bool isMethodOptions() const;

        Q_INVOKABLE bool canMethodHead();
        Q_INVOKABLE bool canMethodGet();
        Q_INVOKABLE bool canMethodPost();
        Q_INVOKABLE bool canMethodPut();
        Q_INVOKABLE bool canMethodDelete();
        Q_INVOKABLE bool canMethodUpload();
        Q_INVOKABLE bool canMethodOptions();

        /**
         * @brief start
         */
        virtual void start();

        /**
         * @brief data
         * @return
         */
        virtual void *data() const;
        virtual void setData(void *data);

        /**
         * @brief codeOption
         * @return
         */
        QRPCListenRequestCode&codeOption();

        /**
         * @brief co
         * @return
         */
        QRPCListenRequestCode&co();
        QRPCListenRequestCode&co(const QVariant &v);

        /**
         * @brief hash
         * @return
         */
        QByteArray hash() const;

        /**
         * @brief isValid
         * @return
         */
        Q_INVOKABLE bool isValid() const;

        /**
         * @brief isEmpty
         * @return
         */
        Q_INVOKABLE bool isEmpty()const;

        /**
         * @brief clear
         */
        Q_INVOKABLE void clear();

        /**
         * @brief fromRequest
         * @param request
         * @return
         */
        bool fromRequest(const QRPCListenRequest &request);

        /**
         * @brief fromMap
         * @param v
         * @return
         */
        bool fromMap(const QVariantMap &vRequest);

        /**
         * @brief mergeMap
         * @param vRequest
         * @return
         */
        bool mergeMap(const QVariantMap &vRequest);

        /**
         * @brief fromMap
         * @param v
         * @return
         */
        bool fromHash(const QVariantHash &vRequest);

        /**
         * @brief mergeHash
         * @param vRequest
         * @return
         */
        bool mergeHash(const QVariantHash &vRequest);

        /**
         * @brief fromResponseMap
         * @param v
         * @return
         */
        bool fromResponseMap(const QVariantHash &vRequest);

        /**
         * @brief fromResponseHash
         * @param v
         * @return
         */
        bool fromResponseHash(const QVariantHash &vRequest);

        /**
         * @brief fromJson
         * @param bytes
         * @return
         */
        bool fromJson(const QByteArray&bytes);

        /**
         * @brief toJson
         * @return
         */
        QByteArray toJson() const;

        /**
         * @brief toMap
         * @return
         */
        QVariantMap toMap() const;

        /**
         * @brief toHash
         * @return
         */
        QVariantHash toHash() const;

        /**
         * @brief requestStart
         * @return
         */
        void requestStart();

        /**
         * @brief listenUuid
         * @return
         */
        QUuid&listenUuid() const;

        /**
         * @brief setListenUuid
         * @param value
         */
        void setListenUuid(const QUuid &value);

        /**
         * @brief requestUuid
         * @return
         */
        QUuid&requestUuid() const;

        /**
         * @brief setRequestUuid
         * @param requestUuid
         */
        void setRequestUuid(const QUuid &value);

        /**
         * @brief requestProtocol
         * @return
         */
        QByteArray&requestProtocol() const;

        /**
         * @brief setRequestProtocol
         * @param value
         */
        void setRequestProtocol(const QVariant &value);

        /**
         * @brief requestPath
         * @return
         */
        QByteArray&requestPath() const;

        /**
         * @brief setRequestPath
         * @param value
         */
        void setRequestPath(const QVariant &value);

        /**
         * @brief requestMethod
         * @return
         */
        QByteArray&requestMethod() const;
        /**
         * @brief setRequestMethod
         * @param value
         */
        void setRequestMethod(const QVariant &value);

        /**
         * @brief requestHeader
         * @return
         */
        QVariantHash&requestHeader() const;

        /**
         * @brief setRequestHeader
         * @param value
         */
        void setRequestHeader(const QVariantHash &value);

        /**
         * @brief requestParameter
         * @return
         */
        QVariantHash&requestParameter() const;

        /**
         * @brief setRequestParameter
         * @param value
         */
        void setRequestParameter(const QVariantHash &value);

        /**
         * @brief requestBody
         * @return
         */
        QVariant&requestBody() const;

        /**
         * @brief requestBodyMap
         * @return
         */
        QVariantMap requestBodyMap() const;

        /**
         * @brief requestBodyHash
         * @return
         */
        QVariantHash requestBodyHash() const;

        /**
         * @brief requestParserBodyMap
         * @param property
         * @param body
         * @return
         */
        bool requestParserBodyMap();
        bool requestParserBodyMap(const QVariant &property);
        bool requestParserBodyMap(const QVariant &property, QVariantMap&body)const;
        bool requestParserBodyHash(const QVariant &property, QVariantHash&body)const;

        /**
         * @brief requestParserProperty
         * @return
         */
        QVariantList &requestParserProperty();

        /**
         * @brief requestParamMap
         * @return
         */
        QVariant requestParamMap(const QByteArray&key) const;
        QVariantMap requestParamMap() const;

        /**
         * @brief requestParamHash
         * @param key
         * @return
         */
        QVariant requestParamHash(const QByteArray&key) const;
        QVariantHash requestParamHash() const;

        /**
         * @brief requestBodyList
         * @return
         */
        QVariantList requestBodyList() const;

        /**
         * @brief setRequestBody
         * @param value
         */
        void setRequestBody(const QVariant &value);

        /**
         * @brief requestTimeout
         * @return
         */
        int requestTimeout() const;

        /**
         * @brief setRequestTimeout
         * @param value
         */
        void setRequestTimeout(int value);

        /**
         * @brief uploadedFiles
         * @return
         */
        QString uploadedFile() const;
        QHash<QString, QFile *>&uploadedFiles() const;
private:
        /**
         * @brief setUploadedFiles
         * @param files
         */
        void setUploadedFiles(const QVariant &vFiles);
public:

        /**
         * @brief temporaryFile
         * @param fileName
         * @return
         */
        QFile *temporaryFile(const QString&fileName);

        /**
         * @brief responseHeader
         * @return
         */
        QVariantHash&responseHeader() const;
        void setResponseHeader(const QVariantHash &value);
        void setResponseHeader(const QRPCHttpHeaders &value);

        /**
         * @brief responseCallback()
         * @return
         */
        QVariantHash&responseCallback() const;
        void setResponseCallback(const QVariantHash &value);

        /**
         * @brief responseBody
         * @return
         */
        QVariant &responseBody() const;
        QByteArray responseBodyBytes() const;
        void setResponseBody(const QVariant &value);

        /**
         * @brief responseCode
         * @return
         */
        int responseCode(int code=-1) const;
        void setResponseCode(const int &value);

        /**
         * @brief responsePhrase
         * @return
         */
        QByteArray responsePhrase() const;
        QByteArray responsePhrase(const int code) const;
        void setResponsePhrase(const QByteArray &value);

        /**
         * @brief requestContentType
         * @return
         */
        int requestContentType() const;
        void setRequestContentType(const QVariant &value);

        /**
         * @brief setRequestResponse
         * @param request
         */
        void setRequestResponse(QObject*request);

        /**
         * @brief setControllerSetting
         * @param setting
         */
        void setControllerSetting(const ControllerSetting&setting);


        /**
         * @brief makeUuid
         */
        const QUuid makeUuid();

    private:
        void*p=nullptr;
    signals:
        /**
         * @brief requestFinish
         */
        void finish();
    };
}
