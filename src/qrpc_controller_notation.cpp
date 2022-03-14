#include "./qrpc_controller_notation.h"

namespace QRpc{

namespace Private{

const QVariantHash &___opHead()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Head}};
    return __return;
}

const QVariantHash &___opGet()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Get}};
    return __return;
}

const QVariantHash &___opPost()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Post}};
    return __return;
}

const QVariantHash &___opPut()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Put}};
    return __return;
}

const QVariantHash &___opDelete()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Delete}};
    return __return;
}

const QVariantHash &___opOptions()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Options}};
    return __return;
}

const QVariantHash &___opPatch()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Patch}};
    return __return;
}

const QVariantHash &___opTrace()
{
    static auto __return =QVariantHash{{QStringLiteral("operation"), QRpc::Trace}};
    return __return;
}

const QVariantList &___opCrud()
{
    static QVariantList __return ={___opGet(), ___opPost(), ___opPost(), ___opDelete()};
    return __return;
}

const QVariantList &___opPostPut()
{
    static auto __return=QVariantList{___opPost(), ___opPut()};
    return __return;
}

}

NotationsExtended::NotationsExtended(QObject *parent)
{
    Q_UNUSED(parent)
}

NotationsExtended::~NotationsExtended()
{

}

}
