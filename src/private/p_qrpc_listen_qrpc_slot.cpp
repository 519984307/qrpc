#include "./p_qrpc_listen_qrpc.h"
#include "./p_qrpc_listen_qrpc_slot.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_parser.h"
#include "../qrpc_controller.h"
#include "../qrpc_controller_router.h"
#include "../qrpc_server.h"
#include <QMutex>
#include <QMetaMethod>
#include <QDebug>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenQRPCSlotPvt*>(this->p)

class QRPCListenQRPCSlotPvt:public QObject{
public:
    bool locked=false;
    QMutex lockedMutex;
    QRPCControllerRouter*controllerRouter=nullptr;
    QRPCListenQRPC*listenQRPC=nullptr;
    QList<const QMetaObject*> listenQRPCControllers;
    QList<const QMetaObject*> listenQRPCParserRequest;
    explicit QRPCListenQRPCSlotPvt(QRPCListenQRPCSlot*slot, QRPCListenQRPC*listenQRPC) : QObject(slot){
        QObject::connect(slot, &QRPCListenQRPCSlot::requestInvoke, this, &QRPCListenQRPCSlotPvt::onRequestInvoke);
        this->listenQRPC = listenQRPC;
        this->listenQRPCControllers=listenQRPC->server()->controllers();
        this->listenQRPCParserRequest=listenQRPC->server()->parsers();
        this->controllerRouter=QRPCControllerRouter::newRouter(this);
    }
    virtual ~QRPCListenQRPCSlotPvt(){
    }

    void invokeController(QRPCListenRequest&request){
        request.co().setMethodNotAllowed();
        //auto requestHash=request.hash();
        QMetaMethod metaMethod;
        auto requestPath=request.requestPath().toLower();
        //if(!QRPCController::routeExists(requestPath) && !QRPCController::routeRedirectCheckRoute("",requestPath)){
        //    request.co().setNotFound();
        //}
        //else
        {
            for(auto&mObjController:this->listenQRPCControllers){
                auto className=mObjController->className();
                auto routeMethods = QRPCController::routeMethods(className);
                if(!routeMethods.contains(requestPath)){//se nao contiver a rota
                    auto routeRedirect = QRPCController::routeRedirectCheckRoute(className, requestPath);
                    if(!routeRedirect){
#if Q_RPC_LOG_SUPER_VERBOSE
                        sWarning()<<"Interface className ignored "<<className;
#endif
                        continue;
                    }
                }

                QScopedPointer<QObject> sObj(mObjController->newInstance(Q_ARG(QObject*, this )));
                if(sObj.data()==nullptr){
#if Q_RPC_LOG
                    sWarning()<<qsl("Interface not valid ")<<className;
#endif
                    continue;
                }
                else{
                    QObject*object=sObj.data();
                    auto controller=dynamic_cast<QRPCController*>(object);

                    if(controller!=nullptr){
                        controller->setRequest(request);
                        controller->setServer(this->listenQRPC->server());
                    }

                    request.co().setOK();

                    if((controller!=nullptr) && (controller->requestRedirect())){
                        break;
                    }
                    else if(!request.co().isOK()){
                        break;
                    }
                    else{
                        metaMethod = routeMethods.value(requestPath);
                        if(!controller->routeRedirectMethod(className, requestPath, metaMethod)){
                            if(request.co().isOK())
                                request.co().setNotFound();
                        }
                        else if(!metaMethod.isValid()){
                            if(request.co().isOK())
                                request.co().setNotFound();
                        }
                        else{
                            QVariant returnVariant;
                            auto argReturn=Q_RETURN_ARG(QVariant, returnVariant);
                            auto parameterType=metaMethod.parameterType(0);
                            auto parameterCount=metaMethod.parameterCount();
                            auto returnType=metaMethod.returnType();

                            QVariant vArgValue;
                            if(parameterCount==1){
                                if(parameterType==QMetaType::QVariantMap)
                                    vArgValue=request.toHash();
                                else if(parameterType==QMetaType::QVariantHash)
                                    vArgValue=QVariant(request.toHash()).toHash();
                                else
                                    vArgValue=QVariant::fromValue<QRPCListenRequest*>(&request);
                            }

                            static const auto sQVariant=qbl("QVariant");
                            auto invokeArg0=QGenericArgument(sQVariant, &vArgValue);
                            bool invokeResult=false;
                            bool goInvoke=true;
                            if(controller!=nullptr){

                                goInvoke=false;
                                if(!controller->requestBeforeInvoke()){
                                    if(controller->rq().co().isOK()){
                                        controller->rq().co().setBadRequest();
                                    }
                                }
                                else{
                                    goInvoke=true;
                                }

                                if(goInvoke){

                                    if(controller->canAuthorization()){
                                        if(!controller->beforeAuthorization()){
                                            sWarning()<<qsl("authorization in Controller::beforeAuthorization()");
                                            if(controller->rq().co().isOK())
                                                controller->rq().co().setUnauthorized();
                                            goInvoke=false;
                                        }
                                        else if(!controller->authorization()){
                                            sWarning()<<qsl("authorization in Controller::authorization()");
                                            if(controller->rq().co().isOK())
                                                controller->rq().co().setUnauthorized();
                                            goInvoke=false;
                                        }
                                        else if(!controller->afterAuthorization()){
                                            sWarning()<<qsl("authorization in Controller::afterAuthorization()");
                                            if(controller->rq().co().isOK())
                                                controller->rq().co().setUnauthorized();
                                            goInvoke=false;
                                        }
                                    }
                                }
                            }

                            if(goInvoke){
                                for(auto&mObjParser:this->listenQRPCParserRequest){

                                    QMetaMethod methodParse;

                                    if(!QRPCListenRequestParser::routeToMethod(*mObjParser, request.requestPath(), methodParse))
                                        continue;
                                    else if(!methodParse.isValid())
                                        continue;
                                    else{
                                        QScopedPointer<QObject> sObj(mObjParser->newInstance(Q_ARG(QObject*, controller )));
                                        if(sObj.data()==nullptr){
#if Q_RPC_LOG
                                            sWarning()<<qsl("Parser not valid ")<<className;
#endif
                                            continue;
                                        }
                                        else{
                                            auto object=sObj.data();
                                            auto parser=dynamic_cast<QRPCListenRequestParser*>(object);
                                            if(parser!=nullptr){
                                                parser->setController(controller);
                                                if(!parser->parse(methodParse)){
#if Q_RPC_LOG
                                                    sWarning()<<qsl("Listen request parser fail: parser ")<<mObjParser->className();
#endif
                                                    goInvoke=false;
                                                    if(request.co().isOK())
                                                        request.co().setNotAcceptable(qsl_null);
                                                }
                                                else{
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if(!goInvoke){
                                if(request.co().isOK())
                                    request.co().setInternalServerError();
                            }
                            else if(!this->controllerRouter->router(&request, metaMethod)){
                                request.co().setCode(this->controllerRouter->lr().sc());
                                if(request.co().isOK())
                                    request.co().setNotFound();
                            }
                            else{
                                if(returnType==QMetaType::Void){
                                    if(parameterCount==0)
                                        invokeResult=metaMethod.invoke(object, Qt::DirectConnection);
                                    else
                                        invokeResult=metaMethod.invoke(object, Qt::DirectConnection, invokeArg0);
                                }
                                else{
                                    if(parameterCount==0)
                                        invokeResult=metaMethod.invoke(object, Qt::DirectConnection, argReturn);
                                    else
                                        invokeResult=metaMethod.invoke(object, Qt::DirectConnection, argReturn, invokeArg0);
                                }
                            }

                            if(controller!=nullptr){
                                controller->requestAfterInvoke();
                            }
                            if(!goInvoke && request.co().isOK()){
                                if(request.co().isOK())
                                    request.co().setInternalServerError();
                            }
                            else if(!invokeResult){
                                if(request.co().isOK())
                                    request.co().setInternalServerError();
                            }
                            else if(returnType!=QMetaType::Void){
                                request.setResponseBody(returnVariant);
                            }
                            break;
                        }
                    }
                }
            }

        }


}

private slots:
void onRequestInvoke(QVariantHash vRequestMap, const QVariant&uploadedFiles){
    if(this->listenQRPC==nullptr){
        qFatal("listen pool is nullptr");
    }
    else{
        auto requestPath=vRequestMap.value(qsl("requestPath")).toString();
        const auto&controllerSetting=this->listenQRPC->server()->controllerOptions().setting(requestPath);
        QRPCListenRequest request(vRequestMap, controllerSetting);
        request.setUploadedFiles(uploadedFiles);
        if(!request.isValid())
            request.co().setBadRequest();
        else
            this->invokeController(request);

        auto listenUuid=request.listenUuid();
        auto requestUuid=request.requestUuid();
        vRequestMap=request.toHash();
        auto listen=this->listenQRPC->childrenListen(listenUuid);
        if(listen==nullptr){
            sWarning()<<qsl("invalid listen for ")<<listenUuid.toString();
        }
        else{
            emit listen->rpcResponse(requestUuid, vRequestMap);
        }
        this->lockedMutex.unlock();
        this->locked=false;
    }
}
};

QRPCListenQRPCSlot::QRPCListenQRPCSlot(QRPCListenQRPC *listenQRPC):QThread(nullptr)
{
    this->p = new QRPCListenQRPCSlotPvt(this, listenQRPC);
}

QRPCListenQRPCSlot::~QRPCListenQRPCSlot()
{

}

void QRPCListenQRPCSlot::run()
{
    QThread::run();
}

bool QRPCListenQRPCSlot::canRequestInvoke(QVariantHash&v, const QVariant &uploadedFiles)
{
    dPvt();
    if(p.locked)
        return false;
    else if(!p.lockedMutex.tryLock(1))
        return false;
    else{
        p.locked=true;
        this->start();
        emit requestInvoke(v, uploadedFiles);
        return true;
    }
}

void QRPCListenQRPCSlot::start()
{
    QThread::start();
    while(this->eventDispatcher()==nullptr){
        QThread::msleep(1);
    }
}

bool QRPCListenQRPCSlot::lock()
{
    dPvt();
    if(p.lockedMutex.tryLock(1)){
        return true;
    }
    else{
        return false;
    }
}

void QRPCListenQRPCSlot::unlock()
{
    dPvt();
    p.lockedMutex.tryLock(1);
    p.lockedMutex.unlock();
}

}
