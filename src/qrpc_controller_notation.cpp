#include "./qrpc_controller_notation.h"

namespace QRpc{

NotationsExtended::NotationsExtended(QObject *parent)
{
    Q_UNUSED(parent)
}

NotationsExtended::~NotationsExtended()
{

}


}


const QVariantHash &Private::___opHead()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Head}};
    return __return;
}

const QVariantHash &Private::___opGet()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Get}};
    return __return;
}

const QVariantHash &Private::___opPost()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Post}};
    return __return;
}

const QVariantHash &Private::___opPut()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Put}};
    return __return;
}

const QVariantHash &Private::___opDelete()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Delete}};
    return __return;
}

const QVariantHash &Private::___opOptions()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Options}};
    return __return;
}

const QVariantHash &Private::___opPatch()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Patch}};
    return __return;
}

const QVariantHash &Private::___opTrace()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Trace}};
    return __return;
}

const QVariantList &Private::___opCrud()
{
    static QVariantList __return ={___opGet(), ___opPost(), ___opPost(), ___opDelete()};
    return __return;
}

const QVariantList &Private::___opPostPut()
{
    static auto __return=QVariantList{___opPost(), ___opPut()};
    return __return;
}
