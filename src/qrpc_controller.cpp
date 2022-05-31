#include "./qrpc_controller.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_listen_request_parser.h"
#include "./qrpc_request.h"
#include "./qrpc_server.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QList>
#include <QMetaMethod>
#include <QMutex>
#include <QVector>

namespace QRpc {

typedef QMultiHash<QByteArray, QRpc::MethodsMultHash> MultStringMethod;

#define dPvt() auto &p = *reinterpret_cast<ControllerPvt *>(this->p)

Q_GLOBAL_STATIC(MetaObjectVector, staticInstalled);
Q_GLOBAL_STATIC(MetaObjectVector, staticParserInstalled);

static void initBasePathParser()
{
    for (auto &metaObject : *staticParserInstalled)
        ListenRequestParser::initializeInstalleds(*metaObject);
}

static void init()
{
    initBasePathParser();
}

Q_COREAPP_STARTUP_FUNCTION(init)

class ControllerPvt
{
public:
    QStringList basePathList;
    Server *server = nullptr;
    ListenRequest *request = nullptr;
    ControllerSetting setting;
    bool enabled = true;

    ListenRequest ____static_request;

    explicit ControllerPvt() {}

    virtual ~ControllerPvt() {}
};

Controller::Controller(QObject *parent) : QObject{parent}, QRpcPrivate::NotationsExtended{this}
{
    this->p = new ControllerPvt();
}

Controller::~Controller()
{
    dPvt();
    delete &p;
}

QStringList &Controller::basePath() const
{
    dPvt();
    if(!p.basePathList.isEmpty())
        return p.basePathList;

    auto &notations=this->notation();
    const auto &notation = notations.find(apiBasePath());
    QVariantList vList;
    if(notation.isValid()){
        auto v = notation.value();
        switch (qTypeId(v)) {
        case QMetaType_QStringList:
        case QMetaType_QVariantList:{
            vList=v.toList();
            break;
        }
        default:
            vList<<v;
        }

        for (auto &row : vList) {
            auto line = row.toString().trimmed().toLower();
            if (line.isEmpty())
                continue;
            p.basePathList<<line;
        }
    }
    if(p.basePathList.isEmpty())
        p.basePathList<<QStringList{qsl("/")};
    return p.basePathList;
}

QString Controller::module() const
{
    auto &notations=this->notation();
    const auto &notation = notations.find(apiModule());
    if(notation.isValid())
        return notation.value().toString();
    return {};
}

QUuid Controller::moduleUuid() const
{
    return {};
}

QString Controller::description() const
{
    const auto &notations = this->notation();
    const auto &notation = notations.find(apiDescription());

    if(!notation.isValid())
        return notation.value().toString();

    return {};
}

ControllerSetting &Controller::setting()
{
    dPvt();
    return p.setting;
}

bool Controller::enabled() const
{
    dPvt();
    return p.enabled;
}

void Controller::setEnabled(bool enabled)
{
    dPvt();
    p.enabled = enabled;
}

ListenRequest &Controller::request()
{
    dPvt();
    return *p.request;
}

ListenRequest &Controller::rq()
{
    dPvt();
    return (p.request == nullptr) ? (p.____static_request) : (*p.request);
}

bool Controller::requestSettings()
{
    auto &rq = this->rq();
    auto vHearder = rq.requestHeader();
    {
        QVariantHash vHash;
        QHashIterator<QString, QVariant> i(vHearder);
        while (i.hasNext()) {
            i.next();
            vHash.insert(QRpc::Util::headerFormatName(i.key()), i.value());
        }
        vHash.remove(qsl("host"));
        vHearder = vHash;
    }

    QVariantHash vHearderResponse;
    auto insertHeaderResponse = [&vHearderResponse](const QString &v0, const QString &v1) {
        const auto key = QRpc::Util::headerFormatName(v0);
        const auto value = v1.trimmed();
        if (!key.isEmpty() && !value.isEmpty())
            vHearderResponse.insert(key, value);
    };

    auto header_application_json = vHearder.value(ContentTypeName).toString().trimmed();
    auto origin = vHearder.value(qsl("Origin")).toString();
    auto referer = vHearder.value(qsl("Referer")).toString();
    auto connectionType = vHearder.value(qsl("Connection-Type")).toString();
    auto connection = vHearder.value(qsl("Connection")).toString();
    auto accessControlRequestHeaders = vHearder.value(qsl("Access-Control-Request-Headers"))
                                           .toString();
    auto accessControlAllowMethods = vHearder.value(qsl("Access-Control-Request-Method")).toString();
    auto accessControlAllowOrigin = origin.isEmpty() ? referer : origin;
    auto keepAlive = vHearder.value(qsl("Keep-Alive")).toString();

    if (rq.isMethodOptions()) {
        //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //https://developer.mozilla.org/pt-BR/docs/Web/HTTP/Methods/OPTIONS
        header_application_json = (!header_application_json.isEmpty()) ? header_application_json
                                                                       : qsl("text/javascripton");
        insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
        insertHeaderResponse(qsl("Server"), this->server()->serverName());
        insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
        insertHeaderResponse(qsl("Access-Control-Allow-Methods"), accessControlAllowMethods);
        insertHeaderResponse(qsl("Access-Control-Allow-Headers"), accessControlRequestHeaders);
        insertHeaderResponse(qsl("Access-Control-Max-Age"), qsl("86400"));
        insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
        insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
        insertHeaderResponse(qsl("Connection"), connection);

        rq.setResponseHeader(vHearderResponse);
        rq.co().setOK();
        return rq.co().isOK();
    }

    //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
    header_application_json = (!header_application_json.isEmpty()) ? header_application_json
                                                                   : qsl("text/javascripton");
    insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
    insertHeaderResponse(qsl("Server"), this->server()->serverName());
    insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
    insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
    insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
    insertHeaderResponse(qsl("Connection"), connection);
    insertHeaderResponse(qsl("Connection-Type"), connectionType);

    rq.setResponseHeader(vHearderResponse);
    rq.co().setOK();
    return rq.co().isOK();
}

bool Controller::canOperation(const QMetaMethod &method)
{
    dPvt();
    if (p.request == nullptr)
        return {};

    if (this->rq().isMethodOptions())
        return true;

    auto &rq = *p.request;
    const auto &notations = this->notation(method);

    if(notations.isEmpty())
        return true;

    if (notations.contains(rqRedirect))
        return true;

    if (notations.contains(rqExcludePath))
        return false;

    if (!notations.containsClassification(ApiOperation))
        return true;

    if (notations.contains(opCrud))
        return true;

    auto operation = qsl("op%1").arg(QString::fromUtf8(rq.requestMethod())).toLower();
    if (notations.contains(operation))
        return true;

    return {};
}

bool Controller::canAuthorization()
{
    if (this->rq().isMethodOptions())
        return true;

    const auto &notations = this->notation();

    if (!notations.containsClassification(Security))
        return true;

    if (notations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::canAuthorization(const QMetaMethod &method)
{
    if (this->rq().isMethodOptions())
        return true;

    const auto &notations = this->notation(method);

    if (!notations.containsClassification(Security))
        return true;

    if (notations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::beforeAuthorization()
{
    return true;
}

bool Controller::authorization()
{
    return true;
}

bool Controller::authorization(const QMetaMethod &method)
{
    const auto &notations = this->notation(method);
    if (!notations.containsClassification(Security))
        return true;

    if (notations.contains(this->rqSecurityIgnore))
        return true;

    return false;
}

bool Controller::afterAuthorization()
{
    return true;
}

bool Controller::requestBeforeInvoke()
{
    return true;
}

bool Controller::requestAfterInvoke()
{
    return true;
}

Server *Controller::server()
{
    dPvt();
    return p.server;
}

const QMetaObject &Controller::install(const QMetaObject &metaObject)
{
    if (!staticInstalled->contains(&metaObject)) {
#if Q_RPC_LOG_VERBOSE
        if (staticInstalled->isEmpty())
            sInfo() << qsl("interface registered: ") << metaObject.className();
        qInfo() << qbl("interface: ") + metaObject.className();
#endif
        (*staticInstalled) << &metaObject;
    }
    return metaObject;
}

const QMetaObject &Controller::installParser(const QMetaObject &metaObject)
{
    if (!staticParserInstalled->contains(&metaObject)) {
#if Q_RPC_LOG_VERBOSE
        if (staticParserInstalled->isEmpty())
            sInfo() << qsl("parser interface registered: ") << metaObject.className();
        qInfo() << qbl("parser interface") + metaObject.className();
#endif
        (*staticParserInstalled) << &metaObject;
    }
    return metaObject;
}

QVector<const QMetaObject *> &Controller::staticApiList()
{
    return *staticInstalled;
}

QVector<const QMetaObject *> &Controller::staticApiParserList()
{
    return *staticParserInstalled;
}

Controller &Controller::setServer(Server *server)
{
    dPvt();
    p.server = server;
    return *this;
}

Controller &Controller::setRequest(ListenRequest &request)
{
    dPvt();
    p.request = &request;
    return *this;
}

} // namespace QRpc
