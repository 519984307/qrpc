#include "./qrpc_listen_request_parser.h"
#include "./private/p_qrpc_listen_request_code.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_controller.h"
#include "./qrpc_listen_request.h"
#include <QByteArray>
#include <QDebug>
#include <QMetaMethod>
#include <QMetaType>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QUuid>
#include <QVariantHash>

namespace QRpc {

typedef QHash<QString, QString> StringStringMap;
typedef QMultiHash<QString, QMetaMethod> StringMetaMethod;

Q_GLOBAL_STATIC(StringStringMap, staticMetaObjectRoute)
Q_GLOBAL_STATIC(StringMetaMethod, staticMetaObjectMetaMethod)
Q_GLOBAL_STATIC(QMutex, staticMetaObjectLock)

#define dPvt() auto &p = *reinterpret_cast<ListenRequestParserPvt *>(this->p)

class ListenRequestParserPvt
{
public:
    Controller *controller = nullptr;
    explicit ListenRequestParserPvt(QObject *parent = nullptr) { Q_UNUSED(parent) }

    virtual ~ListenRequestParserPvt() {}
};

ListenRequestParser::ListenRequestParser(QObject *parent) : QObject{parent}
{
    this->p = new ListenRequestParserPvt(parent);
}

ListenRequestParser::~ListenRequestParser()
{
    dPvt();
    delete &p;
}

Controller &ListenRequestParser::controller()
{
    dPvt();
    return *p.controller;
}

ListenRequest &ListenRequestParser::request()
{
    dPvt();
    if (p.controller == nullptr) {
        static ListenRequest req;
        req.clear();
        return req;
    }
    return p.controller->request();
}

ListenRequest &ListenRequestParser::rq()
{
    return this->request();
}

bool ListenRequestParser::canRoute(const QMetaObject &metaObject, const QString &route)
{
    auto className = QByteArray(metaObject.className());
    QString staticRoute = staticMetaObjectRoute->value(className);
    auto route1 = QRpc::Util::routeParser(staticRoute).toLower();
    auto route2 = QRpc::Util::routeExtract(route).toLower();
    if (route == route2 || route == route2 || route1 == route2)
        return true;
    return false;
}

bool ListenRequestParser::routeToMethod(const QMetaObject &metaObject,
                                        const QString &route,
                                        QMetaMethod &outMethod)
{
    auto vRoute = QRpc::Util::routeParser(route).split(qbl("/")).join(qbl("/"));
    if (ListenRequestParser::canRoute(metaObject, vRoute)) {
        auto list = staticMetaObjectMetaMethod->values(metaObject.className());
        auto v0 = QRpc::Util::routeExtractMethod(route);
        for (auto &v : list) {
            auto v1 = QByteArray(v.name()).toLower();
            if (v0 != v1)
                continue;
            outMethod = v;
            return outMethod.isValid();
        }
    }
    outMethod = QMetaMethod();
    return false;
}

void ListenRequestParser::initializeInstalleds(const QMetaObject &metaObject)
{
    QScopedPointer<QObject> scopePointer(metaObject.newInstance(Q_ARG(QObject*, nullptr)));
    auto object = scopePointer.data();
    if (object == nullptr)
        return;

    auto parser = dynamic_cast<ListenRequestParser *>(object);
    if (parser == nullptr)
        return;

    static const auto ignoreNames = QStringList{qsl("route"), qsl("initializeInstalleds")};
    auto className = QByteArray(metaObject.className());
    if (staticMetaObjectRoute->contains(className))
        return;

    auto route = parser->basePath();

    QMutexLOCKER locker(staticMetaObjectLock);
    staticMetaObjectRoute->insert(className, route);
    for (int methodIndex = 0; methodIndex < metaObject.methodCount(); ++methodIndex) {
        auto method = metaObject.method(methodIndex);
        if (method.returnType() != QMetaType_Bool)
            continue;

        if (method.parameterCount() > 0)
            continue;

        if (ignoreNames.contains(method.name()))
            continue;

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        staticMetaObjectMetaMethod->insert(className, method);
#else
        staticMetaObjectMetaMethod->insertMulti(className, method);
#endif
    }
}

bool ListenRequestParser::parse(const QMetaMethod &metaMethod)
{
    bool returnVariant = false;
    auto argReturn = Q_RETURN_ARG(bool, returnVariant);
    const auto &className = this->metaObject()->className();

    if (!metaMethod.invoke(this, Qt::DirectConnection, argReturn)) {
#if Q_RPC_LOG
        sWarning() << "Parser not valid " << className;
#endif
        return this->rq().co().setInternalServerError().isOK();
    }

    if (!returnVariant) { //Unauthorized
#if Q_RPC_LOG
        sWarning() << "Parser validation error " << className;
#endif
        return this->rq().co().setInternalServerError().isOK();
    }

    return true;
}

void ListenRequestParser::setController(Controller *value)
{
    dPvt();
    p.controller = value;
}

} // namespace QRpc
