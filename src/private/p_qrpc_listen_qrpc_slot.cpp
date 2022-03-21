#include "./p_qrpc_listen_qrpc_slot.h"
#include "../qrpc_controller.h"
#include "../qrpc_controller_router.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_parser.h"
#include "../qrpc_server.h"
#include "./p_qrpc_listen_qrpc.h"
#include <QDebug>
#include <QMetaMethod>
#include <QMutex>

namespace QRpc {

#define dPvt() auto &p = *reinterpret_cast<ListenQRPCSlotPvt *>(this->p)

class ListenQRPCSlotPvt : public QObject
{
public:
    bool locked = false;
    QMutex lockedMutex;
    ControllerRouter *controllerRouter = nullptr;
    ListenQRPC *listenQRPC = nullptr;
    QList<const QMetaObject *> listenControllers;
    QList<const QMetaObject *> listenQRPCParserRequest;
    ControllerMethodCollection controllerMethods;

    explicit ListenQRPCSlotPvt(ListenQRPCSlot *slot, ListenQRPC *listenQRPC) : QObject(slot)
    {
        QObject::connect(slot,
                         &ListenQRPCSlot::requestInvoke,
                         this,
                         &ListenQRPCSlotPvt::onRequestInvoke);
        this->listenQRPC = listenQRPC;
        this->listenControllers = listenQRPC->controllers().values();
        this->listenQRPCParserRequest = listenQRPC->controllerParsers().values();
        this->controllerMethods=listenQRPC->controllerMethods();
        this->controllerRouter = ControllerRouter::newRouter(this);
    }

    virtual ~ListenQRPCSlotPvt() {}


    bool invokeSecurity(Controller *controller, QMetaMethod &metaMethod)
    {
        if (controller == nullptr) {
            return true;
        }

        if (!controller->canAuthorization() || !controller->canAuthorization(metaMethod)) {
#if Q_RPC_LOG_VERBOSE
            sWarning() << qsl("canAuthorization::authorization skipped");
#endif
            return false;
        }

        if (!controller->beforeAuthorization()) {
#if Q_RPC_LOG
            sWarning() << qsl("authorization in Controller::beforeAuthorization()");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setUnauthorized();
            return {};
        }

        if (!controller->authorization()) {
#if Q_RPC_LOG
            sWarning() << qsl("authorization in Controller::authorization()");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setUnauthorized();
            return {};
        }

        if (!controller->afterAuthorization()) {
#if Q_RPC_LOG
            sWarning() << qsl("authorization in Controller::afterAuthorization()");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setUnauthorized();
            return {};
        }
        return true;
    }

    bool invokeParsers(Controller *controller, ListenRequest &request)
    {
        for (auto &mObjParser : this->listenQRPCParserRequest) {
            QMetaMethod methodParse;

            if (!ListenRequestParser::routeToMethod(*mObjParser, request.requestPath(), methodParse))
                continue;

            if (!methodParse.isValid())
                continue;

            QScopedPointer<QObject> sObj(mObjParser->newInstance(Q_ARG(QObject*, controller)));
            auto object = sObj.data();
            if (object == nullptr) {
#if Q_RPC_LOG
                sWarning() << qsl("Parser not valid ") << mObjParser->className();
#endif
                continue;
            }

            auto parser = dynamic_cast<ListenRequestParser *>(object);
            if (parser == nullptr) {
#if Q_RPC_LOG
                sWarning() << qsl("Parser not valid ") << mObjParser->className();
#endif
                delete object;
                continue;
            }

            parser->setController(controller);
            if (parser->parse(methodParse))
                continue;

#if Q_RPC_LOG
            sWarning() << qsl("Listen request parser fail: parser ") << mObjParser->className();
#endif
            if (request.co().isOK())
                request.co().setNotAcceptable(qsl_null);

            return false;
        }
        return true;
    }

    bool invokeRouters(ListenRequest &request, QMetaMethod &metaMethod)
    {
        if (!this->controllerRouter->router(&request, metaMethod)) {
#if Q_RPC_LOG
            sWarning() << qsl("fail: invokeRouters");
#endif
            request.co().setCode(this->controllerRouter->lr().sc());
            if (request.co().isOK())
                request.co().setNotFound();
            return {};
        }
        return true;
    }

    QList<const QMetaObject *> invkeControllers(const QByteArray &requestPath)
    {
        QList<const QMetaObject *> __return;
        for (auto &mObjController : this->listenControllers) {
            auto className = QByteArray(mObjController->className()).toLower();
            auto routeMethods = this->controllerMethods.value(className);
            if (routeMethods.contains(requestPath)){
                __return<<mObjController;
                continue;
            }

            auto routeRedirect = Controller::redirectCheckBasePath(className, requestPath);
            if (routeRedirect){
                __return<<mObjController;
                continue;
            }
        }
        return __return;
    }


    bool invokeQMetaObject(QObject *parentController,
                           ListenRequest &request,
                           Controller *&outController,
                           QMetaMethod &outMetaMethod)
    {
        outController = nullptr;
        outMetaMethod = {};

        if (parentController == nullptr)
            return {};

        if (this->listenControllers.isEmpty()) {
#if Q_RPC_LOG
            sWarning() << qsl("No active controllers");
#endif
            return false;
        }

        const auto requestPath = request.requestPath().toLower();

        auto listenControllers=invkeControllers(requestPath);
        for (auto &mObjController : listenControllers) {
            auto className = QByteArray(mObjController->className()).toLower();
            auto routeMethods = this->controllerMethods.value(className);
            if (!routeMethods.contains(requestPath)) {
                auto routeRedirect = Controller::redirectCheckBasePath(className, requestPath);
                if (!routeRedirect)
                    continue;
            }
            auto object = mObjController->newInstance(Q_ARG(QObject*, parentController));

            if (object == nullptr)
                continue;

            outController = dynamic_cast<Controller *>(object);
            if (outController == nullptr) {
                delete object;
                continue;
            }

            outMetaMethod = routeMethods.value(requestPath);
            if (!outController->redirectMethod(className, requestPath, outMetaMethod)) {
#if Q_RPC_LOG_VERBOSE
                sWarning() << qsl("redirectMethod:: fail");
#endif
                if (request.co().isOK())
                    request.co().setNotFound();
                return {};
            }

            if (!outMetaMethod.isValid()) {
#if Q_RPC_LOG
                sWarning() << qsl("QMetaMethod is not valid:: fail");
#endif
                if (request.co().isOK())
                    request.co().setNotFound();
                return {};
            }

            outController->setRequest(request);
            outController->setServer(this->listenQRPC->server());

            request.co().setOK();

            return true;
        }

        return {};
    }

    bool invokeControllerSettings(Controller *controller)
    {
        if (controller == nullptr)
            return {};

        if (!controller->requestSettings()) {
#if Q_RPC_LOG_VERBOSE
            sWarning() << qsl("requestBeforeInvoke:: fail");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setBadRequest();
            return {};
        }

        return true;
    }

    bool invokeControllerRoutines(Controller *controller, QMetaMethod &metaMethod)
    {
        if (controller == nullptr) {
            return {};
        }

        if (controller->requestRedirect()) {
#if Q_RPC_LOG_VERBOSE
            sWarning() << qsl("requestRedirect:: fail");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setInternalServerError();
            return {};
        }

        if (!controller->canOperation(metaMethod)) {
#if Q_RPC_LOG_VERBOSE
            sWarning() << qsl("canOperation:: fail");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setNotAcceptable();
            return {};
        }

        return true;
    }

    bool invokeMethod(Controller *controller, ListenRequest &request, QMetaMethod &metaMethod)
    {
        QVariant returnVariant;
        auto argReturn = Q_RETURN_ARG(QVariant, returnVariant);
        auto parameterType = metaMethod.parameterType(0);
        auto parameterCount = metaMethod.parameterCount();
        auto returnType = metaMethod.returnType();

        QVariant vArgValue;
        if (parameterCount == 1) {
            switch (parameterType) {
            case QMetaType_QVariantMap:
            case QMetaType_QVariantHash:
                vArgValue = request.toHash();
                break;
            default:
                vArgValue = QVariant::fromValue<ListenRequest *>(&request);
            }
        }

        static const auto sQVariant = qbl("QVariant");
        auto invokeArg0 = QGenericArgument(sQVariant, &vArgValue);
        bool invokeResult = false;

        switch (returnType) {
        case QMetaType_Void: {
            if (parameterCount == 0)
                invokeResult = metaMethod.invoke(controller, Qt::DirectConnection);
            else
                invokeResult = metaMethod.invoke(controller, Qt::DirectConnection, invokeArg0);
            break;
        }
        default:
            if (parameterCount == 0)
                invokeResult = metaMethod.invoke(controller, Qt::DirectConnection, argReturn);
            else
                invokeResult = metaMethod.invoke(controller,
                                                 Qt::DirectConnection,
                                                 argReturn,
                                                 invokeArg0);
        }



        if (!invokeResult) {
            if (request.co().isOK())
                request.co().setInternalServerError();
            return false;
        }

        switch (returnType) {
        case QMetaType_Void:
            request.setResponseBody({});
            break;
        default:
            request.setResponseBody(returnVariant);
        }
        return true;
    }

    void invokeController(ListenRequest &request)
    {
        request.co().setMethodNotAllowed();

        QScopedPointer<QObject> sObj(QObject::staticMetaObject.newInstance(Q_ARG(QObject*, this)));
        QMetaMethod metaMethod;
        Controller *controller = nullptr;

        auto preInvoke=[this, &sObj, &request, &controller, &metaMethod](){
            if (!invokeQMetaObject(sObj.data(), request, controller, metaMethod))
                return false;

            if (!invokeControllerSettings(controller))
                return false;

            if (controller->rq().isMethodOptions())
                return false;

            if (!invokeControllerRoutines(controller, metaMethod))
                return false;

            return true;
        };

        auto invokeMethods=[this, &request, &controller, &metaMethod]()
        {
            if (!this->invokeSecurity(controller, metaMethod))
                return false;

            if (!this->invokeParsers(controller, request))
                return false;

            if (!this->invokeRouters(request, metaMethod))
                return false;

            if (!this->invokeMethod(controller, request, metaMethod))
                return false;

            return true;
        };

        if(!preInvoke())
            return;

        if(controller==nullptr){
#if Q_RPC_LOG_VERBOSE
            sWarning() << qsl("Invalid controller");
#endif
            return;
        }

        if (!controller->requestBeforeInvoke()) {
#if Q_RPC_LOG_VERBOSE
            sWarning() << qsl("requestBeforeInvoke:: fail");
#endif
            if (controller->rq().co().isOK())
                controller->rq().co().setInternalServerError();
            return;
        }

        if(!invokeMethods())
            return;

        controller->requestAfterInvoke();
    }

private slots:
    void onRequestInvoke(QVariantHash vRequestHash, const QVariant &uploadedFiles)
    {
        if (this->listenQRPC == nullptr)
            qFatal("listen pool is nullptr");

        auto requestPath = vRequestHash.value(qsl("requestPath")).toString();
        const auto &controllerSetting = this->listenQRPC->server()->controllerOptions().setting(
            requestPath);
        ListenRequest request(vRequestHash, controllerSetting);
        request.setUploadedFiles(uploadedFiles);
        if (!request.isValid())
            request.co().setBadRequest();
        else
            this->invokeController(request);

        auto listenUuid = request.listenUuid();
        auto requestUuid = request.requestUuid();
        vRequestHash = request.toHash();
        auto listen = this->listenQRPC->childrenListen(listenUuid);
        if (listen == nullptr) {
            sWarning() << qsl("invalid listen for ") << listenUuid.toString();
        } else {
            emit listen->rpcResponse(requestUuid, vRequestHash);
        }
        this->lockedMutex.unlock();
        this->locked = false;
    }
};

ListenQRPCSlot::ListenQRPCSlot(ListenQRPC *listenQRPC) : QThread{nullptr}
{
    this->p = new ListenQRPCSlotPvt(this, listenQRPC);
}

ListenQRPCSlot::~ListenQRPCSlot() {}

void ListenQRPCSlot::run()
{
    QThread::run();
}

bool ListenQRPCSlot::canRequestInvoke(QVariantHash &v, const QVariant &uploadedFiles)
{
    dPvt();
    if (p.locked)
        return false;

    if (!p.lockedMutex.tryLock(1))
        return false;

    p.locked = true;
    this->start();
    emit requestInvoke(v, uploadedFiles);
    return true;
}

void ListenQRPCSlot::start()
{
    QThread::start();
    while (this->eventDispatcher() == nullptr)
        QThread::msleep(1);
}

bool ListenQRPCSlot::lock()
{
    dPvt();
    if (p.lockedMutex.tryLock(1))
        return true;
    return false;
}

void ListenQRPCSlot::unlock()
{
    dPvt();
    p.lockedMutex.tryLock(1);
    p.lockedMutex.unlock();
}

} // namespace QRpc
